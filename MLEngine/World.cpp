#include "World.h"
#include "LevelStreaming.h"
#include "GameMode.h"
#include "MLEngine.h"
#include "ViewMatrices.h"
#include "MeshBatch.h"
#include "Level.h"
#include "Actor.h"

namespace ML
{
	CWorld::CWorld(const std::string& name)
	{
		Name = name;
	}

	CWorld::CWorld(std::string&& name) noexcept
	{
		Name = std::move(name);
	}

	CWorld::~CWorld()
	{
		// 只有在确认渲染线程已经停帧之后才会走到这里（见 CEngine::UnInitialise）。
		Scene.Release();
	}


	void CWorld::LoadAsync(const std::string& FilePath, FOnSceneLoaded OnLoaded /*= nullptr*/)
	{
		PendingFilePath = FilePath;
		OnLoadedFunc = OnLoaded;
		LoadState = EWorldLoadState::Parsing;
		GEngine->threadDetach([this]()
			{
				LoadPersistentLevel();
				// GPU 上传必须主线程做，所以这里只标记
				if (LoadState != EWorldLoadState::Failed)
				{
					LoadState = EWorldLoadState::UploadingGPU;
				}
			});
	}

	// UE counterpart: UWorld loads the map package and takes ownership of the
	// resulting ULevel. Deserializing the actors themselves is CLevel's job
	// (ULevel::Serialize); the world only picks up what belongs to it - here,
	// the streaming level declarations and the world name.
	void CWorld::LoadPersistentLevel()
	{
		if (PendingFilePath.empty())
		{
			OutputDebugStringA("CWorld::LoadPersistentLevel: PendingFilePath is empty\n");
			LoadState = EWorldLoadState::Failed;
			return;
		}

		if (nullptr == PersistentLevel)
		{
			PersistentLevel = new CLevel();
		}

		// Streaming levels are UWorld data, but they are declared in the same map
		// package, so CLevel::Load hands them back instead of storing them.
		std::vector<FStreamingLevelDesc> StreamingDescs;
		if (!PersistentLevel->Load(PendingFilePath, &StreamingDescs))
		{
			OutputDebugStringA(("CWorld::LoadPersistentLevel: failed to load " + PendingFilePath + "\n").c_str());
			LoadState = EWorldLoadState::Failed;
			return;
		}

		// UE: the world is named after the map it was loaded from.
		// Name lives on CObject, so it goes through SetName.
		if (!PersistentLevel->GetName().empty())
		{
			SetName(PersistentLevel->GetName());
		}

		LoadState = EWorldLoadState::BuildingActors;

		// UE: UWorld::StreamingLevels (ULevelStreaming) - registered, not loaded here.
		for (const FStreamingLevelDesc& Desc : StreamingDescs)
		{
			AddStreamingLevel(Desc.Name, Desc.Path);
		}
	}

	void CWorld::CollectVisibleMeshes(const FViewMatrices& View, std::vector<FMeshBatch>& OutBatches)
	{
		// 渲染线程。顺序很关键：
		//   1) 先把游戏线程投递过来的更新（Add / SetTransform / SetVisibility）
		//      应用到场景镜像上 —— 对应 UE 渲染线程帧首执行渲染命令队列；
		//   2) 再从镜像里收集批次。
		//
		// 这样渲染线程全程只碰 FScene / FPrimitiveSceneProxy，不再读
		// PersistentLevel 里的 CActor / CComponent（它们归游戏线程所有，
		// 主线程 Tick 会改它们的 Transform、也会增删容器元素）。
		RenderCommands.Drain(Scene);
		Scene.CollectVisibleMeshes(View, OutBatches);
	}

	void CWorld::TickLoad()
	{
		if (LoadState == EWorldLoadState::UploadingGPU)
		{
			UploadGPUResources();
			InitActorsForPlay();
			LoadState = EWorldLoadState::Ready;
			if(OnLoadedFunc)
			{
				OnLoadedFunc(this);
			}
		}

	}

	void CWorld::Tick(float deltaSeconds)
	{
		if (PersistentLevel)
		{
			for (CActor* Actor : PersistentLevel->GetActors())
			{
				if (Actor && Actor->CanTick())
				{
					Actor->Tick(deltaSeconds);

					// 顺带 Tick 组件
					for (CComponent* Comp : Actor->GetComponents())
					{
						Comp->tick(deltaSeconds);
					}
				}
			}
		}
		for (CLevelStreaming* Stream : StreamingLevels)
		{
			if (Stream && Stream->IsLoaded())
			{
				CLevel* Lv = Stream->GetLevel();
				for (CActor* Actor : Lv->GetActors())
				{
					if (Actor && Actor->CanTick())
					{
						Actor->Tick(deltaSeconds);
					}
				}
			}
		}
	}

	CLevelStreaming* CWorld::AddStreamingLevel(const std::string& LevelName, const std::string& FilePath)
	{
		CLevelStreaming* LevelStreaming = new CLevelStreaming();
        //LevelStreaming->Initialise(LevelName, FilePath);
        StreamingLevels.push_back(LevelStreaming);
        return LevelStreaming;
	}

	void CWorld::RemoveStreamingLevel(const std::string& LevelName)
	{
		for (auto iter = StreamingLevels.begin(); iter != StreamingLevels.end(); ++iter)
		{
			if ((*iter)->GetLevelName() == LevelName)
			{
				StreamingLevels.erase(iter);
				break;
			}		
		}
	}

	CLevelStreaming* CWorld::GetStreamingLevel(const std::string& LevelName) const
	{
		for (CLevelStreaming* LevelSteam : StreamingLevels)
		{
			if (LevelSteam->GetLevelName() == LevelName)
			{
				return LevelSteam;
			}
		}
		return nullptr;
	}

	void CWorld::InitActorsForPlay()
	{
		if (!PersistentLevel)
		{
			return;
		}
		for (CActor* Actor : PersistentLevel->GetActors())
		{
			if (nullptr == Actor)
			{
				continue;
			}

			// BeginPlay 可能改 Transform —— 此时组件还没注册（RenderProxyId == 0），
			// 那些改动会被直接丢弃；紧接着下面创建的 proxy 会带上最终值，不会丢。
			Actor->BeginPlay();

			// UE: AActor::RegisterAllComponents → UActorComponent::RegisterComponentWithWorld。
			// 纯多态分发：CWorld 不认识任何具体组件类型 —— 以后新增可绘制组件
			// 只需覆写 CreateSceneProxy，这里一行不用改。
			for (CComponent* Comp : Actor->GetComponents())
			{
				if (Comp)
				{
					Comp->RegisterComponentWithWorld(this);
				}
			}
		}
	}

	void CWorld::UploadGPUResources()
	{

	}

}
