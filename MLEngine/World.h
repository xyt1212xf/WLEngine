#pragma once
#include "Resource.h"
#include "Array.h"

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

	class CWorld : public CResource
	{
        friend class CEngine;
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

		std::string GetName() const { return Name; }

		CLevelStreaming* AddStreamingLevel(const std::string& LevelName, const std::string& FilePath);
		void RemoveStreamingLevel(const std::string& LevelName);
		CLevelStreaming* GetStreamingLevel(const std::string& LevelName) const;
		const TArray<CLevelStreaming*>& GetStreamingLevels() const { return StreamingLevels; }

	private:
		void CollectVisibleMeshes(const FViewMatrices& View, TArray<FMeshBatch>& OutBatches);

		// 主线程里调用
		void TickLoad();

		void Tick(float deltaSeconds);

		void ParseFile();
		void BuildActors();
		void InitActorsForPlay();
		void UploadGPUResources();

	private:
		std::string PendingFilePath = "";
		std::string Name = "";
		bool bLoaded = false;
		bool bVisible = false;

		CGameMode* GameMode = nullptr;
		CLevel* PersistentLevel = nullptr;
		TArray<CLevelStreaming*> StreamingLevels;
		EWorldLoadState LoadState = EWorldLoadState::Idle;
		FOnSceneLoaded OnLoadedFunc;
		TArray<CActor*>	ParseActors;
	};
}