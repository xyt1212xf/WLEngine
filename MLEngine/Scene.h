#pragma once
#include "Common.h"
#include "Matrix.h"
#include "MeshBatch.h"

namespace ML
{
	class CStaticMeshComponent;
	class CStaticMesh;
	class CStaticMeshLODResources;
	class CStaticVertexFactory;
	class CMaterial;
	struct FViewMatrices;

	// 分配一个全局唯一的图元 id。
	// 由游戏线程在 CreateSceneProxy 时调用：必须先拿到 id，之后针对该图元的
	// SetTransform / SetVisibility 命令才能正确投递（命令是 FIFO 的，
	// 保证 Add 一定排在它的更新之前）。
	MLENGINE_API uint32 AllocateSceneProxyId();

	//====================================================================
	// UE: FPrimitiveSceneProxy
	//
	// 渲染线程侧对一个可绘制组件的"镜像"。
	// 由游戏线程一次性快照构造，之后**只有渲染线程**读写它；游戏线程对组件
	// 的任何改动都通过 FRenderCommandQueue 单向投递过来。
	//
	// 这一层存在的理由：CActor / CComponent 归游戏线程所有，主线程 Tick 会
	// 改它们的 Transform、也会增删容器元素。渲染线程若直接遍历它们，就会
	// 出现"读撕裂的矩阵"和"迭代器失效（vector 重分配）"两类崩溃。
	//====================================================================
	class MLENGINE_API FPrimitiveSceneProxy
	{
		friend class FScene;
	public:
		// 游戏线程调用：把组件此刻的可绘制状态整份拷出来。
		FPrimitiveSceneProxy(uint32 InId, const CStaticMeshComponent* InComponent);

	private:
		// ---- 以下只允许渲染线程调用 ----
		uint32 GetId() const { return Id; }
		bool   IsVisible() const { return bVisible; }

		void SetWorldMatrix(const FMatrix44& InWorldMatrix) { WorldMatrix = InWorldMatrix; }
		void SetVisible(bool bInVisible) { bVisible = bInVisible; }

		// UE: FPrimitiveSceneProxy::GetDynamicMeshElements
		// 渲染线程：把镜像展开成绘制批次。构造时数据已定，这里只做填充。
		bool GetMeshBatch(FMeshBatch& OutBatch) const;

	private:
		uint32 Id = 0;

		// ---- 快照（游戏线程写入，渲染线程只读）----
		FMatrix44				WorldMatrix;
		CStaticMesh*			Mesh = nullptr;
		CStaticMeshLODResources* LOD = nullptr;
		CStaticVertexFactory*	VertexFactory = nullptr;
		CMaterial*				Material = nullptr;   // 构造时已解析（override 优先）
		FMeshBatchElement		Element;
		uint32					SectionIndex = 0;
		bool					bVisible = false;
	};

	//====================================================================
	// UE: FScene
	// 渲染线程拥有的场景状态。图元注册/注销/更新全部走这里。
	//====================================================================
	class MLENGINE_API FScene
	{
	public:
		~FScene();

		// 幂等：同一 id 重复注册会先移除旧的，避免泄漏。
		void AddPrimitive(FPrimitiveSceneProxy* Proxy);
		void RemovePrimitive(uint32 ProxyId);
		// 批量注销（渲染线程）。典型用途：卸关卡时一条命令注销全部 proxy，
		// 免去逐个投递。const& —— 不要按值拷贝 id 列表。
		void RemovePrimitives(const std::vector<uint32>& ProxyIds);

		void SetTransform(uint32 ProxyId, const FMatrix44& InWorldMatrix);
		void SetVisibility(uint32 ProxyId, bool bInVisible);

		// 渲染线程：把可见图元展开成绘制批次。
		// View 目前仅占位，P1 在这里做视锥/距离剔除。
		void CollectVisibleMeshes(const FViewMatrices& View, std::vector<FMeshBatch>& OutBatches) const;

		int32 GetNumPrimitives() const { return (int32)Proxies.size(); }

		// 删除全部图元（只在确认渲染线程不再访问时调用）。
		void Release();

	private:
		FPrimitiveSceneProxy* Find(uint32 ProxyId) const;

		// vector 保证遍历顺序稳定（有利于按材质聚类，减少 PSO 切换）；
		// IndexById 让按 id 的更新是 O(1)。
		std::vector<FPrimitiveSceneProxy*>		Proxies;
		std::unordered_map<uint32, size_t>		IndexById;
	};

	//====================================================================
	// UE: ENQUEUE_RENDER_COMMAND
	//
	// 游戏线程投递、渲染线程执行的单向命令队列。
	// 执行过程在锁外进行：游戏线程只在"交换缓冲区"那一瞬间被阻塞，
	// 不会因为渲染线程执行了一堆命令而卡住。
	//====================================================================
	class MLENGINE_API FRenderCommandQueue
	{
	public:
		using FCommand = std::function<void(FScene&)>;

		void Enqueue(FCommand InCommand);

		// 渲染线程在帧首调用：取出全部待处理命令并就地执行。
		void Drain(FScene& Scene);

	private:
		std::mutex				Mutex;
		std::vector<FCommand>	Pending;
	};
}
