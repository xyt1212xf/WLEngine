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

	CPrimitiveSceneProxy::CPrimitiveSceneProxy(uint32 InId, CMesh* InMesh, CMeshLODResources* InLOD, CVertexStreamFactory* InVF, CMaterial* InMaterial, const FMatrix44& InWorldMatrix, uint32 InSectionIndex /*= 0*/, bool bInVisible /*= true*/)
	: Id(InId)
	{
		Mesh = InMesh;
		LOD = InLOD;
		VertexFactory = InVF;
		Material = InMaterial;
		WorldMatrix = InWorldMatrix;
		SectionIndex = InSectionIndex;
		bVisible = bInVisible;
		if (LOD)
		{
			Element.FirstIndex = 0;
			Element.NumIndices = LOD->NumIndices;
			Element.BaseVertex = 0;
			Element.InstanceCount = 1;
		}
	}

	bool CPrimitiveSceneProxy::GetMeshBatch(FMeshBatch& OutBatch) const
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
	CScene::~CScene()
	{
		Release();
	}

	CPrimitiveSceneProxy* CScene::Find(uint32 ProxyId) const
	{
		auto It = IndexById.find(ProxyId);
		if (It == IndexById.end())
		{
			return nullptr;
		}
		const std::vector<CPrimitiveSceneProxy*>& Vec = Bucket(It->second.first);
		return Vec[It->second.second];
	}

	CScene::EProxyBucket CScene::Classify(const CMaterial* Material)
	{
		if (Material && Material->IsSky())
		{
			return EProxyBucket::Sky;
		}
		if (Material && Material->IsTranslucent())
		{
			return EProxyBucket::Translucent;
		}
		return EProxyBucket::Opaque;
	}

	std::vector<ML::CPrimitiveSceneProxy*>& CScene::Bucket(EProxyBucket Tag)
	{
		switch (Tag)
		{
		case EProxyBucket::Sky:         return SkyProxies;
		case EProxyBucket::Translucent: return TranslucentProxies;
		default:                        return OpaqueProxies;
		}
	}

	const std::vector<ML::CPrimitiveSceneProxy*>& CScene::Bucket(EProxyBucket Tag) const
	{
		switch (Tag)
		{
		case EProxyBucket::Sky:         return SkyProxies;
		case EProxyBucket::Translucent: return TranslucentProxies;
		default:                        return OpaqueProxies;
		}
	}

	void CScene::RebuildIndex()
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
		for (size_t i = 0; i < SkyProxies.size(); ++i)
		{
			IndexById[SkyProxies[i]->GetId()] = { EProxyBucket::Sky, i };
		}
	}

	void CScene::AddPrimitive(CPrimitiveSceneProxy* Proxy)
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
		const EProxyBucket BucketTag = Classify(Proxy->Material);
		IndexById[Proxy->GetId()] = { BucketTag, Bucket(BucketTag).size() };
		Bucket(BucketTag).push_back(Proxy);
	}

	void CScene::RemovePrimitive(uint32 ProxyId)
	{
		auto It = IndexById.find(ProxyId);
		if (It == IndexById.end())
		{
			return;
		}

		std::vector<CPrimitiveSceneProxy*>& Vec = Bucket(It->second.first);
		const size_t Index = It->second.second;

		// 防御：下标越界或槽位 id 不符时，三桶按 id 线性找回（逻辑同原版）。
		if (Index >= Vec.size() || nullptr == Vec[Index] || Vec[Index]->GetId() != ProxyId)
		{
			bool bFound = false;
			for (EProxyBucket Tag : { EProxyBucket::Opaque, EProxyBucket::Translucent, EProxyBucket::Sky })
			{
				std::vector<CPrimitiveSceneProxy*>& pBucket = Bucket(Tag);
				for (size_t i = 0; i < pBucket.size(); ++i)
				{
					if (pBucket[i] && pBucket[i]->GetId() == ProxyId)
					{
						SafeDelete(pBucket[i]);
						pBucket.erase(pBucket.begin() + i);
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
		RebuildIndex();
	}


	void CScene::RemovePrimitives(const std::vector<uint32>& ProxyIds)
	{
		bool bAnyRemoved = false;
		for (uint32 Id : ProxyIds)
		{
			auto It = IndexById.find(Id);
			if (It == IndexById.end())
			{
				continue;
			}
			std::vector<CPrimitiveSceneProxy*>& Vec = Bucket(It->second.first);
			SafeDelete(Vec[It->second.second]);
			Vec[It->second.second] = nullptr;
			IndexById.erase(It);
			bAnyRemoved = true;
		}

		if (bAnyRemoved)
		{
			for (EProxyBucket Tag : { EProxyBucket::Opaque, EProxyBucket::Translucent, EProxyBucket::Sky })
			{
				std::vector<CPrimitiveSceneProxy*>& pBucket = Bucket(Tag);
				pBucket.erase(std::remove(pBucket.begin(), pBucket.end(), nullptr), pBucket.end());
			}
			RebuildIndex();
		}
	}

	void CScene::ReclassifyPrimitive(uint32 ProxyId)
	{
		auto It = IndexById.find(ProxyId);
		if (It == IndexById.end())
		{
			return;
		}

		const EProxyBucket OldTag = It->second.first;
		std::vector<CPrimitiveSceneProxy*>& OldVec = Bucket(OldTag);

		CPrimitiveSceneProxy* Proxy = nullptr;
		if (It->second.second < OldVec.size() && OldVec[It->second.second] &&
			OldVec[It->second.second]->GetId() == ProxyId)
		{
			Proxy = OldVec[It->second.second];
		}
		else
		{
			for (EProxyBucket Tag : { EProxyBucket::Opaque, EProxyBucket::Translucent, EProxyBucket::Sky })
			{
				for (CPrimitiveSceneProxy* P : Bucket(Tag))
				{
					if (P && P->GetId() == ProxyId)
					{
						Proxy = P;
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
			return;
		}

		const EProxyBucket NewTag = Classify(Proxy->Material);
		if (NewTag == OldTag)
		{
			return;
		}

		auto Pos = std::find(OldVec.begin(), OldVec.end(), Proxy);
		if (Pos == OldVec.end())
		{
			return;
		}
		OldVec.erase(Pos);
		Bucket(NewTag).push_back(Proxy);
		RebuildIndex();
	}

	void CScene::ReclassifyPrimitives(const std::vector<uint32>& ProxyIds)
	{
		if (ProxyIds.empty())
		{
			return;
		}

		struct FMove
		{
			CPrimitiveSceneProxy* Proxy;
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

			const EProxyBucket OldBucket = It->second.first;
			std::vector<CPrimitiveSceneProxy*>& pOldVec = Bucket(OldBucket);

			CPrimitiveSceneProxy* Proxy = nullptr;
			if (It->second.second < pOldVec.size() && pOldVec[It->second.second] &&
				pOldVec[It->second.second]->GetId() == Id)
			{
				Proxy = pOldVec[It->second.second];
			}
			else
			{
				for (EProxyBucket Tag : { EProxyBucket::Opaque, EProxyBucket::Translucent, EProxyBucket::Sky })
				{
					for (CPrimitiveSceneProxy* P : Bucket(Tag))
					{
						if (P && P->GetId() == Id) { Proxy = P; break; }
					}
					if (Proxy) break;
				}
			}
			if (nullptr == Proxy)
			{
				continue;
			}

			const EProxyBucket NewBucket = Classify(Proxy->Material);
			if (NewBucket == OldBucket)
			{
				continue;
			}

			IndexById.erase(It);
			Moves.push_back({ Proxy, NewBucket });
		}

		if (Moves.empty())
		{
			return;
		}

		for (const FMove& M : Moves)
		{
			for (EProxyBucket Tag : { EProxyBucket::Opaque, EProxyBucket::Translucent, EProxyBucket::Sky })
			{
				auto Pos = std::find(Bucket(Tag).begin(), Bucket(Tag).end(), M.Proxy);
				if (Pos != Bucket(Tag).end())
				{
					*Pos = nullptr;
					break;
				}
			}
		}
		for (EProxyBucket Tag : { EProxyBucket::Opaque, EProxyBucket::Translucent, EProxyBucket::Sky })
		{
			std::vector<CPrimitiveSceneProxy*>& pBucket = Bucket(Tag);
			pBucket.erase(std::remove(pBucket.begin(), pBucket.end(), nullptr), pBucket.end());
		}
		for (const FMove& M : Moves)
		{
			Bucket(M.NewTag).push_back(M.Proxy);
		}
		RebuildIndex();
	}

	void CScene::SetTransform(uint32 ProxyId, const FMatrix44& InWorldMatrix)
	{
		if (CPrimitiveSceneProxy* Proxy = Find(ProxyId))
		{
			Proxy->SetWorldMatrix(InWorldMatrix);
		}
	}

	void CScene::SetVisibility(uint32 ProxyId, bool bInVisible)
	{
		if (CPrimitiveSceneProxy* Proxy = Find(ProxyId))
		{
			Proxy->SetVisible(bInVisible);
		}
	}

	void CScene::CollectVisibleMeshes(const FViewMatrices& View,
										std::vector<FMeshBatch>& OutSky,
										std::vector<FMeshBatch>& OutOpaque,
										std::vector<FMeshBatch>& OutTranslucent) const
	{
		// P1: 这里放视锥/距离剔除（FViewMatrices 已备好，Bounds 在 LOD 上）。
		(void)View;

		for (const CPrimitiveSceneProxy* Proxy : SkyProxies)
		{
			if (nullptr == Proxy || !Proxy->IsVisible())
			{
				continue;
			}
			FMeshBatch Batch;
			if (Proxy->GetMeshBatch(Batch) && nullptr != Batch.Material)
			{
				OutSky.push_back(Batch);
			}
		}

		for (const CPrimitiveSceneProxy* Proxy : OpaqueProxies)
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
		for (const CPrimitiveSceneProxy* Proxy : TranslucentProxies)
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

	int32 CScene::GetNumPrimitives() const
	{
		return (int32)(OpaqueProxies.size() + TranslucentProxies.size() + SkyProxies.size());
	}

	void CScene::Release()
	{
		for (CPrimitiveSceneProxy* Proxy : OpaqueProxies)
		{
			SafeDelete(Proxy);
		}
		for (CPrimitiveSceneProxy* Proxy : TranslucentProxies)
		{
			SafeDelete(Proxy);
		}
		for (CPrimitiveSceneProxy* Proxy : SkyProxies)
		{
			SafeDelete(Proxy);
		}
		SkyProxies.clear();
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

	void FRenderCommandQueue::Drain(CScene& Scene)
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
