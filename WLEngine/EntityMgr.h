#pragma once
#include "Entity.h"
#include "EntityFactory.h"

namespace WL
{
	class CEntityMgr
	{
	public:
		CEntityMgr();
		virtual ~CEntityMgr();
		bool initialize();
		void unInitialize();
		template<typename T>
		T* createEntity(EntityType type);
		CEntity* createEntity(EntityType type);
		bool loadCameraEntity(CActorEntity* pEntity, std::string szFile);
		bool loadActorEntity(CActorEntity* pEntity, std::string szModel);
		bool loadLightEntity(CActorEntity* pEntity, std::string szFile);
		bool loadSkyEntity(CActorEntity* pEntity, std::string szModel);
		bool loadSkyBoxEntity(CActorEntity* pEntity, std::string szModel);
		bool loadTerrainEntity(CEntity* pEntity, std::string szModel);
		bool loadOceanEntity(CActorEntity* pEntity, std::string szModel);

	private:
		CEntityFactory* mpFactory = nullptr;
	};

	template<typename T>
	T* WL::CEntityMgr::createEntity(EntityType type)
	{
		T* pEntity = dynamic_cast<T*>(mpFactory->createEntity(type));
		return pEntity;
	}
}