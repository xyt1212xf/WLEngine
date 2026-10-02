#pragma once
#include "Resource.h"
#include "Array.h"

namespace ML
{
	class CLevel;
	class CLevelStreaming;
	class CGameMode;
	class CWorld : public CResource
	{
	public:
		CWorld(const std::string& name);
		CWorld(std::string&& name);
		virtual ~CWorld();
		bool Initialise(const std::string& FilePath);
		void Destroy();
		// Ö÷¹Ø¿¨
		CLevel* GetPersistentLevel() const { return PersistentLevel; }

		void Update(float DeltaTime);


		void SetOnLoadedCallback(FOnSceneLoaded Callback) { OnLoaded = Callback; }

		bool IsLoaded() const { return bLoaded; }
		bool IsVisible() const { return bVisible; }

		std::string GetName() const { return Name; }

		CLevelStreaming* AddStreamingLevel(const std::string& LevelName, const std::string& FilePath);
		void RemoveStreamingLevel(const std::string& LevelName);
		CLevelStreaming* GetStreamingLevel(const std::string& LevelName) const;
		const TArray<CLevelStreaming*>& GetStreamingLevels() const { return StreamingLevels; }

	private:
		std::string Name;
		bool bLoaded = false;
		bool bVisible = false;

		CGameMode* GameMode = nullptr;
		CLevel* PersistentLevel = nullptr;
		TArray<CLevelStreaming*> StreamingLevels;

		FOnSceneLoaded OnLoaded;
	};
}