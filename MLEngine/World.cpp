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
				ParseFile();
				BuildActors();
				// GPU 上传必须主线程做，所以这里只标记
				LoadState = EWorldLoadState::UploadingGPU;
			});
	}

	void CWorld::CollectVisibleMeshes(const FViewMatrices& View, TArray<FMeshBatch>& OutBatches)
	{

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
        StreamingLevels.Add(LevelStreaming);
        return LevelStreaming;
	}

	void CWorld::RemoveStreamingLevel(const std::string& LevelName)
	{
		for (CLevelStreaming* LevelSteam : StreamingLevels)
		{
			if (LevelSteam->GetLevelName() == LevelName)
			{
				StreamingLevels.Remove(LevelSteam);
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

	void CWorld::ParseFile()
	{
		// P0 阶段：硬编码生成一个 Cube（后续 P2 由 ParseFile 读关卡文件替代）
		// 注意：mesh 必须由堆对象持有（组件引用它），不能是栈上局部变量
		CStaticMesh* CubeMesh = new CStaticMesh();
		CubeMesh->AddMaterialSlot("Default");
		CubeMesh->GetSourceModel(CubeMesh->AddSourceModel())->CreateCube(Vec3F(0, 0, 0), Vec3F(1, 1, 1));
		CubeMesh->Build();

		CActor* CubeActor = new CActor();
		CStaticMeshComponent* MeshComp = CubeActor->AddComponent<CStaticMeshComponent>();
		MeshComp->SetStaticMesh(CubeMesh);
		MeshComp->SetVisible(true);
		MeshComp->SetLocation(Vec3F(0, 0, 3.f));   // 放到默认相机前方

		ParseActors.Add(CubeActor);
	}

	void CWorld::BuildActors()
	{
		if (!PersistentLevel)
		{
			PersistentLevel = new CLevel();
		}
		for (CActor* Actor : ParseActors)
		{
			PersistentLevel->AddActor(Actor);
		}	
		ParseActors.Empty();
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