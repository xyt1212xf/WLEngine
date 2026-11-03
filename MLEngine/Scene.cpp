#include "Scene.h"
#include "StaticMeshComponent.h"
#include "StaticMesh.h"
#include "StaticMeshResources.h"
#include "ViewMatrices.h"
#include "Material.h"
#include <algorithm>

namespace ML
{
	static std::atomic<uint32> GNextSceneProxyId{ 1 };

	uint32 AllocateSceneProxyId()
	{
		return GNextSceneProxyId.fetch_add(1);
	}

	//-----------------------------------------------------------------
	// FPrimitiveSceneProxy
	//-----------------------------------------------------------------
	FPrimitiveSceneProxy::FPrimitiveSceneProxy(uint32 InId, const CStaticMeshComponent* InComponent)
	: Id(InId)
	{
		if (nullptr == InComponent)
		{
			return;
		}

		Mesh = InComponent->GetStaticMesh();
		if (nullptr == Mesh)
		{
			return;
		}

		// 网格尚未 Build（LOD / VertexFactory 为空，或还没有 Section）时
		// 不产出可绘制数据：LOD 保持 nullptr，GetMeshBatch 会返回 false，
		// 渲染线程自然跳过它。
		if (CStaticMeshLODResources* InLOD = Mesh->GetRenderData(0))
		{
			CStaticVertexFactory* InVF  = Mesh->GetVertexFactory(0);
			if (nullptr == InVF || InLOD->Sections.empty())
			{
				return;
			}
			LOD = InLOD;
			VertexFactory = InVF;

			SectionIndex = 0;
			const FStaticMeshSection& Section = LOD->Sections[0];
			Element.FirstIndex = Section.FirstIndex;
			Element.NumIndices = Section.NumTriangles * 3;
			Element.BaseVertex = 0;
			Element.InstanceCount = 1;

			// 材质在构造时一次性解析好：渲染线程不再需要知道 override 规则。
			// UE: 组件 OverrideMaterials 优先，否则回落到网格资产自己的材质槽。
			Material = InComponent->GetOverrideMaterial(SectionIndex);
			if (nullptr == Material)
			{
				Material = Mesh->GetMaterialForSection(SectionIndex);
			}

			WorldMatrix = InComponent->GetWorldMatrix();
			bVisible = InComponent->IsVisible();
		}
	}

	bool FPrimitiveSceneProxy::GetMeshBatch(FMeshBatch& OutBatch) const
	{
		// LOD / VertexFactory 为空说明网格没烘好，直接跳过。
		if (!bVisible || nullptr == LOD || nullptr == VertexFactory)
		{
			return false;
		}

		OutBatch.Mesh = Mesh;
		OutBatch.LOD = LOD;
		OutBatch.VertexFactory = VertexFactory;
		OutBatch.Material = Material;
		OutBatch.WorldMatrix = WorldMatrix;
		OutBatch.SectionIndex = SectionIndex;
		OutBatch.Element = Element;
		return true;
	}

	//-----------------------------------------------------------------
	// FScene
	//-----------------------------------------------------------------
	FScene::~FScene()
	{
		Release();
	}

	FPrimitiveSceneProxy* FScene::Find(uint32 ProxyId) const
	{
		auto It = IndexById.find(ProxyId);
		if (It == IndexById.end())
		{
			return nullptr;
		}
		const auto& Vec = (It->second.first == EProxyBucket::Translucent) ? TranslucentProxies : OpaqueProxies;
		return Vec[It->second.second];
	}

	void FScene::RebuildIndex()
	{
		IndexById.clear();
		for (size_t i = 0; i < OpaqueProxies.size(); ++i)
		{
			IndexById[OpaqueProxies[i]->GetId()] = { EProxyBucket::Opaque, i };
		}
		for (size_t i = 0; i < TranslucentProxies.size(); ++i)
		{
			IndexById[TranslucentProxies[i]->GetId()] = { EProxyBucket::Translucent, i };
		}
	}

	void FScene::AddPrimitive(FPrimitiveSceneProxy* Proxy)
	{
		if (nullptr == Proxy)
		{
			return;
		}
		// 幂等：同 id 重复注册先移除旧的，否则旧的那份会泄漏。
		RemovePrimitive(Proxy->GetId());

		// 分类在注册这一刻完成：材质是否透明只判断一次，之后两桶各自存放，
		// 渲染时直接按桶取，不再每帧判断（UE: 图元注册时即决定进哪个 mesh-pass 列表）。
		// 无材质的 proxy 归入不透明桶，由 CollectVisibleMeshes 在展开时跳过。
		const bool bTranslucent = (nullptr != Proxy->Material) && Proxy->Material->IsTranslucent();
		std::vector<FPrimitiveSceneProxy*>& Bucket = bTranslucent ? TranslucentProxies : OpaqueProxies;
		const EProxyBucket BucketTag = bTranslucent ? EProxyBucket::Translucent : EProxyBucket::Opaque;

		IndexById[Proxy->GetId()] = { BucketTag, Bucket.size() };
		Bucket.push_back(Proxy);
	}

	void FScene::RemovePrimitive(uint32 ProxyId)
	{
		auto It = IndexById.find(ProxyId);
		if (It == IndexById.end())
		{
			return;
		}

		// 以索引里的"桶 + 下标"定位目标槽位——索引在每次增删后都会 RebuildIndex，
		// 所以它通常精确指向该 proxy 物理上所在的桶。材质可能在运行时在"不透明
		// <-> 透明"之间切换（AddPrimitive 幂等：先 Remove 再用新材质重注册），
		// 但 proxy 实体一直躺在注册那一刻放进去的桶里，摘除只看它实际在哪个桶，
		// 因此信任索引即天然覆盖了切换后的删除。
		auto& Vec = (It->second.first == EProxyBucket::Translucent) ? TranslucentProxies : OpaqueProxies;
		const size_t Index = It->second.second;

		// 防御：下标越界，或槽位已不是本 proxy（标签与实体不一致，例如经历过
		// 透明<->不透明切换且索引未及时刷新）。退化到两桶按 id 线性找回，保证
		// 一定能摘掉，绝不会因过期标签删错别的图元。
		if (Index >= Vec.size() || nullptr == Vec[Index] || Vec[Index]->GetId() != ProxyId)
		{
			bool bFound = false;
			for (std::vector<FPrimitiveSceneProxy*>* pBucket : { &OpaqueProxies, &TranslucentProxies })
			{
				for (size_t i = 0; i < pBucket->size(); ++i)
				{
					if ((*pBucket)[i] && (*pBucket)[i]->GetId() == ProxyId)
					{
						SafeDelete((*pBucket)[i]);
						pBucket->erase(pBucket->begin() + i);
						bFound = true;
						break;
					}
				}
				if (bFound)
				{
					break;
				}
			}
			IndexById.erase(It);
			if (bFound)
			{
				RebuildIndex();
			}
			return;
		}

		SafeDelete(Vec[Index]);
		Vec.erase(Vec.begin() + Index);
		IndexById.erase(It);
		// erase 让该桶后面元素的下标整体左移一位，重建索引表（两个桶一起重建）。
		RebuildIndex();
	}


	void FScene::RemovePrimitives(const std::vector<uint32>& ProxyIds)
	{
		// 渲染线程调用（同 RemovePrimitive）。典型用途：卸一个关卡时用
		// 一条命令批量注销它的全部 proxy，而不是每个 proxy 一条。
		//
		// 批量删除不能"边删边查"：erase 一次，桶里所有更大的下标就整体
		// 失效，下一轮循环再拿查出来的下标去 Vec[Index] 就会删错元素甚至越界。
		// 做法（标记-压缩）：
		//   1) 第一遍只把要删的槽位 SafeDelete 成 nullptr、并就地摘掉索引项
		//      （顺带天然去重：同一 id 第二次查不到，不会用陈旧下标二次删除）；
		//   2) 第二遍把两个桶里的空槽位分别压缩掉，各桶内部相对顺序不变——保持
		//      CollectVisibleMeshes 按材质聚类的遍历稳定性；
		//   3) 下标全变了，重建索引表。
		bool bAnyRemoved = false;
		for (uint32 Id : ProxyIds)
		{
			auto It = IndexById.find(Id);
			if (It == IndexById.end())
			{
				continue;
			}
			std::vector<FPrimitiveSceneProxy*>& Vec =
				(It->second.first == EProxyBucket::Translucent) ? TranslucentProxies : OpaqueProxies;
			SafeDelete(Vec[It->second.second]);   // 槽位置空，兼作删除标记
			Vec[It->second.second] = nullptr;
			IndexById.erase(It);                 // 索引项就地摘除
			bAnyRemoved = true;
		}

		if (bAnyRemoved)
		{
			OpaqueProxies.erase(std::remove(OpaqueProxies.begin(), OpaqueProxies.end(), nullptr), OpaqueProxies.end());
			TranslucentProxies.erase(std::remove(TranslucentProxies.begin(), TranslucentProxies.end(), nullptr), TranslucentProxies.end());
			RebuildIndex();
		}
	}

	void FScene::ReclassifyPrimitive(uint32 ProxyId)
	{
		auto It = IndexById.find(ProxyId);
		if (It == IndexById.end())
		{
			return;
		}

		// 先按索引快速定位（索引在每次增删后都会 RebuildIndex，通常精确指向实体），
		// 槽位上的 id 不匹配时（标签过期，如经历过透明<->不透明切换）退化到两桶按
		// id 找回，保证一定能定位到该 proxy 实际所在的桶。
		const EProxyBucket OldTag = It->second.first;
		auto Proxies = (OldTag == EProxyBucket::Translucent) ? &TranslucentProxies : &OpaqueProxies;

		FPrimitiveSceneProxy* Proxy = nullptr;
		if (It->second.second < Proxies->size() &&
			(*Proxies)[It->second.second] &&
			(*Proxies)[It->second.second]->GetId() == ProxyId)
		{
			Proxy = (*Proxies)[It->second.second];
		}
		else
		{
			for (std::vector<FPrimitiveSceneProxy*>* pBucket : { &OpaqueProxies, &TranslucentProxies })
			{
				for (FPrimitiveSceneProxy* P : *pBucket)
				{
					if (P && P->GetId() == ProxyId) { Proxy = P; Proxies = pBucket; break; }
				}
				if (Proxy) break;
			}
		}
		if (nullptr == Proxy)
		{
			return;
		}

		// 按 proxy 当前材质的透明度重算目标桶（材质混合模式可能在运行时切换）。
		const bool bTranslucent = (nullptr != Proxy->Material) && Proxy->Material->IsTranslucent();
		const EProxyBucket NewTag = bTranslucent ? EProxyBucket::Translucent : EProxyBucket::Opaque;
		if (NewTag == OldTag)
		{
			return;   // 没跨桶，无需搬迁
		}

		// 从旧桶摘除（保持旧桶内部顺序，利于按材质聚类），塞进新桶末尾，重建索引。
		auto Pos = std::find(Proxies->begin(), Proxies->end(), Proxy);
		if (Pos == Proxies->end())
		{
			return;
		}
		Proxies->erase(Pos);
		(NewTag == EProxyBucket::Translucent ? TranslucentProxies : OpaqueProxies).push_back(Proxy);
		RebuildIndex();
	}

	void FScene::ReclassifyPrimitives(const std::vector<uint32>& ProxyIds)
	{
		if (ProxyIds.empty())
		{
			return;
		}

		// 批量版：先统一把"需要跨桶"的 proxy 找出来并记录目标桶，只重建一次索引，
		// 避免逐个 ReclassifyPrimitive 时每次都 O(n) 重建（卸关卡等批量场景明显更快）。
		// 标记-压缩思路同 RemovePrimitives：
		//   1) 第一遍：定位每个 proxy（索引过期则两桶按 id 找回），按当前材质算目标桶，
		//      仅当"实际所在桶 != 目标桶"才摘索引并记入待搬列表（同一 id 第二次查不到，天然去重）；
		//   2) 第二遍：把待搬 proxy 在旧桶里标记 nullptr，压缩两个桶，再追加到目标桶；
		//   3) 下标全变，重建索引。两桶内部相对顺序保持不变（利于按材质聚类）。
		struct FMove
		{
			FPrimitiveSceneProxy* Proxy;
			EProxyBucket NewTag;
		};

		std::vector<FMove> Moves;
		Moves.reserve(ProxyIds.size());

		for (uint32 Id : ProxyIds)
		{
			auto It = IndexById.find(Id);
			if (It == IndexById.end())
			{
				continue;
			}

			// 定位 proxy 实际所在桶（索引过期时退化到两桶按 id 找回）。
			const EProxyBucket OldBucket = It->second.first;
			auto pOldVec = (OldBucket == EProxyBucket::Translucent) ? &TranslucentProxies : &OpaqueProxies;

			FPrimitiveSceneProxy* Proxy = nullptr;
			if (It->second.second < pOldVec->size() &&
				(*pOldVec)[It->second.second] &&
				(*pOldVec)[It->second.second]->GetId() == Id)
			{
				Proxy = (*pOldVec)[It->second.second];
			}
			else
			{
				for (std::vector<FPrimitiveSceneProxy*>* pBucket : { &OpaqueProxies, &TranslucentProxies })
				{
					for (FPrimitiveSceneProxy* P : *pBucket)
					{
						if (P && P->GetId() == Id)
						{
							Proxy = P;
							pOldVec = pBucket;
							break;
						}
					}
					if (Proxy)
					{
						break;
					}
				}
			}
			if (nullptr == Proxy)
			{
				continue;
			}

			// 按 proxy 当前材质重算目标桶；与"实际所在桶"相同则不动（索引保持原样）。
			// 注意：必须用实际所在桶比较，而非索引里记的旧标签——旧标签可能已因之前的
			// 透明<->不透明切换而过期为错，否则会漏搬。
			const bool bTranslucent = (nullptr != Proxy->Material) && Proxy->Material->IsTranslucent();
			const EProxyBucket NewBucket = bTranslucent ? EProxyBucket::Translucent : EProxyBucket::Opaque;
			const EProxyBucket OldBucketActual =
				(pOldVec == &TranslucentProxies) ? EProxyBucket::Translucent : EProxyBucket::Opaque;
			if (NewBucket == OldBucketActual)
			{
				continue;
			}

			IndexById.erase(It);   // 只在确定要搬时摘索引（去重）
			Moves.push_back({ Proxy, NewBucket });
		}

		if (Moves.empty())
		{
			return;
		}

		// 在 proxy 实际所在的旧桶里把它标记成 nullptr（指针比对，不依赖任何下标），
		// 随后统一压缩、并追加到目标桶。
		for (const FMove& M : Moves)
		{
			for (std::vector<FPrimitiveSceneProxy*>* pVec : { &OpaqueProxies, &TranslucentProxies })
			{
				auto Pos = std::find(pVec->begin(), pVec->end(), M.Proxy);
				if (Pos != pVec->end())
				{
					*Pos = nullptr;
					break;
				}
			}
		}
		OpaqueProxies.erase(std::remove(OpaqueProxies.begin(), OpaqueProxies.end(), nullptr), OpaqueProxies.end());
		TranslucentProxies.erase(std::remove(TranslucentProxies.begin(), TranslucentProxies.end(), nullptr), TranslucentProxies.end());
		for (const FMove& M : Moves)
		{
			(M.NewTag == EProxyBucket::Translucent ? TranslucentProxies : OpaqueProxies).push_back(M.Proxy);
		}
		RebuildIndex();
	}

	void FScene::SetTransform(uint32 ProxyId, const FMatrix44& InWorldMatrix)
	{
		if (FPrimitiveSceneProxy* Proxy = Find(ProxyId))
		{
			Proxy->SetWorldMatrix(InWorldMatrix);
		}
	}

	void FScene::SetVisibility(uint32 ProxyId, bool bInVisible)
	{
		if (FPrimitiveSceneProxy* Proxy = Find(ProxyId))
		{
			Proxy->SetVisible(bInVisible);
		}
	}

	void FScene::CollectVisibleMeshes(const FViewMatrices& View,
		std::vector<FMeshBatch>& OutOpaque, std::vector<FMeshBatch>& OutTranslucent) const
	{
		// P1: 这里放视锥/距离剔除（FViewMatrices 已备好，Bounds 在 LOD 上）。
		(void)View;

		// 两个桶本身就是"不透明 / 透明"的分类结果（AddPrimitive 时已按材质分流），
		// 这里直接按桶展开，不再判断材质是否透明。
		for (const FPrimitiveSceneProxy* Proxy : OpaqueProxies)
		{
			if (nullptr == Proxy || !Proxy->IsVisible())
			{
				continue;
			}
			FMeshBatch Batch;
			if (Proxy->GetMeshBatch(Batch) && nullptr != Batch.Material)
			{
				OutOpaque.push_back(Batch);
			}
		}
		for (const FPrimitiveSceneProxy* Proxy : TranslucentProxies)
		{
			if (nullptr == Proxy || !Proxy->IsVisible())
			{
				continue;
			}
			FMeshBatch Batch;
			if (Proxy->GetMeshBatch(Batch) && nullptr != Batch.Material)
			{
				OutTranslucent.push_back(Batch);
			}
		}
	}

	void FScene::Release()
	{
		for (FPrimitiveSceneProxy* Proxy : OpaqueProxies)
		{
			delete Proxy;
		}
		for (FPrimitiveSceneProxy* Proxy : TranslucentProxies)
		{
			delete Proxy;
		}
		OpaqueProxies.clear();
		TranslucentProxies.clear();
		IndexById.clear();
	}

	//-----------------------------------------------------------------
	// FRenderCommandQueue
	//-----------------------------------------------------------------
	void FRenderCommandQueue::Enqueue(FCommand InCommand)
	{
		if (!InCommand)
		{
			return;
		}
		std::lock_guard<std::mutex> Lock(Mutex);
		Pending.push_back(std::move(InCommand));
	}

	void FRenderCommandQueue::Drain(FScene& Scene)
	{
		std::vector<FCommand> Local;
		{
			// 只在交换缓冲区的瞬间持锁
			std::lock_guard<std::mutex> Lock(Mutex);
			Local.swap(Pending);
		}

		// 执行在锁外：游戏线程不会因为我们跑命令而被阻塞
		for (FCommand& Cmd : Local)
		{
			Cmd(Scene);
		}
	}
}
