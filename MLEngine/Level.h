#pragma once
#include "Refcount.h"
#include "Array.h"

namespace ML
{
	class CActor;
	class CResource;
	class AWorldSettings;
	class CLevel : public CRefcount
	{
	public:
		CLevel();
		virtual ~CLevel();

		virtual bool Load(const std::string& FilePath);
		virtual void Unload();

		// Actor 管理
		void AddActor(CActor* Actor);
		void RemoveActor(CActor* Actor);
		const TArray<CActor*>& GetActors() const { return Actors; }

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
		TArray<CActor*> Actors;
		TArray<CResource*> Resources;
	};
}
