#pragma once
#include "Refcount.h"
#include <string>
#include <vector>

namespace ML
{
	class CActor;
	class CResource;
	class AWorldSettings;

	// UE: one entry of UWorld::StreamingLevels (ULevelStreaming).
	// Streaming levels are WORLD data, not level data - they are declared in the
	// same map package, so CLevel::Load hands them back to CWorld to own instead
	// of storing them on the level.
	struct FStreamingLevelDesc
	{
		std::string Name;
		std::string Path;
	};

	class MLENGINE_API CLevel : public CRefcount
	{
	public:
		CLevel();
		virtual ~CLevel();

		// UE: ULevel::Serialize - deserializes this level's actors from the map
		// package. Streaming level declarations live in the same package but
		// belong to CWorld, so they are returned through OutStreamingLevels.
		virtual bool Load(const std::string& FilePath, std::vector<FStreamingLevelDesc>* OutStreamingLevels = nullptr);
		virtual void Unload();

		// Actor 管理
		void AddActor(CActor* Actor);
		void RemoveActor(CActor* Actor);
		const std::vector<CActor*>& GetActors() const { return Actors; }

		// 资源管理
		void RegisterResources();
		void ReleaseResources();

		// 状态
		bool IsLoaded() const { return bLoaded; }
		bool IsVisible() const { return bVisible; }

		std::string GetName() const { return Name; }

	protected:
		std::string Name = "";
		bool bLoaded = false;
		bool bVisible = false;
		AWorldSettings* WorldSettings;  // 关卡规则	
		std::vector<CActor*> Actors;
		std::vector<CResource*> Resources;
	};
}
