#pragma once
#include "Object.h"
#include <string>
#include <vector>
#include <unordered_map>

namespace ML
{
	class CActor;
	class AWorldSettings;
	class CMaterial;

	// UE: one entry of UWorld::StreamingLevels (ULevelStreaming).
	// Streaming levels are WORLD data, not level data - they are declared in the
	// same map package, so CLevel::Load hands them back to CWorld to own instead
	// of storing them on the level.
	struct FStreamingLevelDesc
	{
		std::string Name;
		std::string Path;
	};

	class MLENGINE_API CLevel : public CObject
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

		// 状态
		bool IsLoaded() const { return bLoaded; }
		bool IsVisible() const { return bVisible; }

	protected:
		bool bLoaded = false;
		bool bVisible = false;
		AWorldSettings* WorldSettings;  // 关卡规则	
		std::vector<CActor*> Actors;

		// Materials this level declared inline (see CLevel::Load's Materials table).
		//
		// UE: a level references material ASSETS by path and nothing else - they
		// are loaded through the asset registry, shared, and outlive every level.
		// There are no material .ma assets yet, so whoever declares a material
		// also owns it; that is the level. Keyed by the name OverrideMaterials uses.
		std::unordered_map<std::string, CMaterial*> LevelMaterials;

		// UE: ULevel holds no asset array - referenced assets live in the
		// package/asset registry, not on the level. The old CResource* vector
		// was never populated, so it was removed rather than retyped.
	};
}
