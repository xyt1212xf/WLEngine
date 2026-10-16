#include "World.h"
#include "LevelStreaming.h"
#include "GameMode.h"
#include "MLEngine.h"
#include "ViewMatrices.h"
#include "MeshBatch.h"
#include "Level.h"
#include "Actor.h"
#include "StaticMesh.h"
#include "StaticMeshComponent.h"

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
		if (!PersistentLevel->GetName().empty())
		{
			Name = PersistentLevel->GetName();
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
		// Iterate the persistent level: actor -> component -> visible static meshes.
		// (UE: FScene from FPrimitiveSceneProxy registration; simplified here.)
		if (nullptr == PersistentLevel)
		{
			return;
		}
		for (CActor* Actor : PersistentLevel->GetActors())
		{
			if (nullptr == Actor)
			{
				continue;
			}
			for (CComponent* Comp : Actor->GetComponents())
			{
				CStaticMeshComponent* MeshComp = dynamic_cast<CStaticMeshComponent*>(Comp);
				if (nullptr == MeshComp || !MeshComp->IsVisible())
				{
					continue;
				}
				FMeshBatch Batch;
				MeshComp->GetMeshBatch(Batch, View.ViewProjMatrix);
				// GetMeshBatch leaves LOD null when the mesh is not built yet.
				if (Batch.LOD)
				{
					OutBatches.push_back(Batch);
				}
			}
		}
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
			Actor->BeginPlay();
		}
	}

	void CWorld::UploadGPUResources()
	{

	}

}
