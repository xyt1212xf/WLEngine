#include "World.h"
#include "LevelStreaming.h"

namespace ML
{
	CWorld::CWorld()
	{

	}

	CWorld::~CWorld()
	{

	}

	bool CWorld::Initialise(const std::string& FilePath)
	{
		return true;
	}

	void CWorld::Destroy()
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

}