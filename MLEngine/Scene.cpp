#include "Scene.h"
#include "StaticMeshComponent.h"
#include "StaticMesh.h"
#include "StaticMeshResources.h"
#include "ViewMatrices.h"
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
		return (It == IndexById.end()) ? nullptr : Proxies[It->second];
	}

	void FScene::AddPrimitive(FPrimitiveSceneProxy* Proxy)
	{
		if (nullptr == Proxy)
		{
			return;
		}
		// 幂等：同 id 重复注册先移除旧的，否则旧的那份会泄漏。
		RemovePrimitive(Proxy->GetId());

		IndexById[Proxy->GetId()] = Proxies.size();
		Proxies.push_back(Proxy);
	}

	void FScene::RemovePrimitive(uint32 ProxyId)
	{
		auto It = IndexById.find(ProxyId);
		if (It == IndexById.end())
		{
			return;
		}

		const size_t Index = It->second;
		SafeDelete(Proxies[Index]);
		Proxies.erase(Proxies.begin() + Index);

		// erase 让后面元素的下标整体左移一位，重建索引表。
		IndexById.clear();
		for (size_t i = 0; i < Proxies.size(); ++i)
		{
			IndexById[Proxies[i]->GetId()] = i;
		}
	}


	void FScene::RemovePrimitives(const std::vector<uint32>& ProxyIds)
	{
		// 渲染线程调用（同 RemovePrimitive）。典型用途：卸一个关卡时用
		// 一条命令批量注销它的全部 proxy，而不是每个 proxy 一条。
		//
		// 批量删除不能"边删边查"：erase 一次，IndexById 里所有更大的下标
		// 就整体失效，下一轮循环再拿查出来的下标去 Proxies[Index] 就会
		// 删错元素甚至越界。
		// 做法（标记-压缩）：
		//   1) 第一遍只把要删的槽位 SafeDelete 成 nullptr、并就地摘掉索引项
		//      （顺带天然去重：同一 id 第二次查不到，不会用陈旧下标二次删除）；
		//   2) 第二遍把空槽位压缩掉，剩余元素相对顺序不变——保持
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
			SafeDelete(Proxies[It->second]);   // 槽位置空，兼作删除标记
			IndexById.erase(It);               // 索引项就地摘除
			bAnyRemoved = true;
		}

		if (bAnyRemoved)
		{
			Proxies.erase(std::remove(Proxies.begin(), Proxies.end(), nullptr), Proxies.end());

			IndexById.clear();
			for (size_t i = 0; i < Proxies.size(); ++i)
			{
				IndexById[Proxies[i]->GetId()] = i;
			}
		}
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

	void FScene::CollectVisibleMeshes(const FViewMatrices& View, std::vector<FMeshBatch>& OutBatches) const
	{
		// P1: 这里放视锥/距离剔除（FViewMatrices 已备好，Bounds 在 LOD 上）。
		(void)View;

		for (const FPrimitiveSceneProxy* Proxy : Proxies)
		{
			if (nullptr == Proxy || !Proxy->IsVisible())
			{
				continue;
			}
			FMeshBatch Batch;
			if (Proxy->GetMeshBatch(Batch))
			{
				OutBatches.push_back(Batch);
			}
		}
	}

	void FScene::Release()
	{
		for (FPrimitiveSceneProxy* Proxy : Proxies)
		{
			delete Proxy;
		}
		Proxies.clear();
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
