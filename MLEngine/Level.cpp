#include "Level.h"
#include "WorldSet.h"

namespace ML
{
	CLevel::CLevel()
	{

	}

	CLevel::~CLevel()
	{

	}

	bool CLevel::Load(const std::string& FilePath)
	{
		return true;
	}

	void CLevel::Unload()
	{

	}

	void CLevel::AddActor(CActor* Actor)
	{
		if (Actor)
		{
			Actors.push_back(Actor);
		}
	}

	void CLevel::RemoveActor(CActor* Actor)
	{
		auto item = std::find(Actors.begin(), Actors.end(), Actor);
		if (item != Actors.end())
		{
			Actors.erase(item);
		}
	}

	void CLevel::RegisterResources()
	{

	}

	void CLevel::ReleaseResources()
	{

	}

}
