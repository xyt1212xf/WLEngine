#pragma once
#include "Object.h"
#include "Scene.h"
#include <atomic>

namespace ML
{
	enum class EWorldLoadState : uint8
	{
		Idle,
		Parsing,
		BuildingActors,
		UploadingGPU,
		Ready,
		Failed
	};

	class CLevel;
	class CLevelStreaming;
	class CGameMode;
	class CActor;
	struct FMeshBatch;
	struct FViewMatrices;

	using FOnSceneLoaded = std::function<void(class CWorld*)>;

	class MLENGINE_API CWorld : public CObject
	{
        friend class CEngine;
		friend class CRenderer;

	public:
		CWorld() = default;
		CWorld(const std::string& name);
		CWorld(std::string&& name) noexcept;
		virtual ~CWorld();
	
		// 异步加载
		void LoadAsync( const std::string& FilePath, FOnSceneLoaded OnLoaded = nullptr);


		// 主关卡
		CLevel* GetPersistentLevel() const { return PersistentLevel; }

		void Update(float DeltaTime);

		bool IsReady() const { return LoadState == EWorldLoadState::Ready; }

		void SetOnLoadedCallback(FOnSceneLoaded Callback) { OnLoadedFunc = Callback; }

		bool IsLoaded() const { return bLoaded; }
		bool IsVisible() const { return bVisible; }

		// UE: UWorld is a plain UObject - it carries no asset/streaming payload,
		// so it must not sit on CStreamableRenderAsset. Name comes from CObject.

		// ---- 游戏线程 → 渲染线程 ----
		// UE: ENQUEUE_RENDER_COMMAND。组件改 Transform / 可见性时走这里，
		// 渲染线程在帧首（CollectVisibleMeshes）统一 drain 并执行。
		void EnqueueRenderCommand(std::function<void(FScene&)> InCommand)
		{
			RenderCommands.Enqueue(std::move(InCommand));
		}

		CLevelStreaming* AddStreamingLevel(const std::string& LevelName, const std::string& FilePath);
		void RemoveStreamingLevel(const std::string& LevelName);
		CLevelStreaming* GetStreamingLevel(const std::string& LevelName) const;
		const std::vector<CLevelStreaming*>& GetStreamingLevels() const { return StreamingLevels; }

	private:
		void CollectVisibleMeshes(const FViewMatrices& View, std::vector<FMeshBatch>& OutBatches);

		// 主线程里调用
		void TickLoad();

		void Tick(float deltaSeconds);

		// UE: UWorld loads the map package and owns the resulting ULevel.
		// Deserializing the actors themselves is CLevel::Load (ULevel::Serialize).
		void LoadPersistentLevel();
		void InitActorsForPlay();
		void UploadGPUResources();

	private:
		std::string PendingFilePath = "";
		bool bLoaded = false;
		bool bVisible = false;

		CGameMode* GameMode = nullptr;
		CLevel* PersistentLevel = nullptr;
		std::vector<CLevelStreaming*> StreamingLevels;

		// UE: 异步加载状态标志。worker 线程（LoadAsync）写、主线程每帧读
		// （TickLoad / IsReady），必须是原子量。
		// worker 的 seq_cst store 同时也是"发布"：它建好的 PersistentLevel /
		// Actors 向量对主线程 TickLoad 里匹配的 load 可见，
		// InitActorsForPlay 走 Actor 列表不需要再加锁。
		std::atomic<EWorldLoadState> LoadState = EWorldLoadState::Idle;
		FOnSceneLoaded OnLoadedFunc;

		// ---- 渲染线程侧的场景镜像 ----
		// UE: UWorld 对应一个 FScene。渲染线程只读这两个成员，
		// 绝不去碰 PersistentLevel 里的 CActor / CComponent。
		FScene				Scene;
		FRenderCommandQueue	RenderCommands;
	};
}
