#include "EntityFactory.h"
namespace WL
{
	CEntityFactory::~CEntityFactory()
	{

	}


	CEntity* CEntityFactory::createEntity(EntityType type)
	{
		CEntity* pEntity = nullptr;
		switch (type)
		{
		case Entity:
			pEntity = WL_NEW(CEntity, Entity);
			pEntity->mszName = "Entity";
			break;
		case Camera:
			pEntity = WL_NEW(CCameraEntity, Entity);
			pEntity->mszName = "CameraEntity";
			break;
		case Actor:
			pEntity = WL_NEW(CActorEntity, Entity);
			pEntity->mszName = "ActorEntity";
			break;
		case Light:
			pEntity = WL_NEW(CLightEntity, Entity);
			pEntity->mszName = "LightEntity";
			break;
		case Sky:
			pEntity = WL_NEW(CSkyEntity, Entity);
			pEntity->mszName = "SkyEntity";
			break;
		case Terrain:
			pEntity = WL_NEW(CTerrainEntity, Entity);
			pEntity->mszName = "TerrainEntity";
			break;
		case SkyBox:
			pEntity = WL_NEW(CSkyBoxEntity, Entity);
			pEntity->mszName = "SkyBoxEntity";
			break;
		case Ocean:
			pEntity = WL_NEW(COceanEntity, Entity);
			pEntity->mszName = "OceanEntity";
			break;
		case Gui:
			pEntity = WL_NEW(CGuiEntity, Entity);
			pEntity->mszName = "GuiEntity";
			break;
		default:
			return nullptr;
		}
		pEntity->mType = type;
		return pEntity;
	}

}