#include "World.h"
#include "LevelStreaming.h"
#include "GameMode.h"
#include "MLEngine.h"

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

	}

	void CWorld::BuildActors()
	{

	}

	void CWorld::InitActorsForPlay()
	{

	}

	void CWorld::UploadGPUResources()
	{

	}

}