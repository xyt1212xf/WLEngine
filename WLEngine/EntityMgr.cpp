#include "EntityMgr.h"
#include "WLEngine.h"
#include "SkyEntity.h"
#include "TerrainEntity.h"
#include "DefMemory.h"

namespace WL
{
	CEntityMgr::CEntityMgr()
	{

	}

	CEntityMgr::~CEntityMgr()
	{
	}

	bool CEntityMgr::initialize()
	{
		mpFactory = WL_NEW(CEntityFactory, Factory);
		return true;
	}

	void CEntityMgr::unInitialize()
	{
		WL_DELETE(mpFactory, Factory);
	}


	CEntity* CEntityMgr::createEntity(EntityType type)
	{
		return mpFactory->createEntity(type);
	}

	bool CEntityMgr::loadCameraEntity(CActorEntity* pEntity, std::string szFile)
	{
		return true;
	}

	bool CEntityMgr::loadActorEntity(CActorEntity* pEntity, std::string szModel)
	{
		auto pModel = dynamic_cast<CModel*>(GEngine->createResource(szModel, Model));
		if (nullptr != pModel)
		{
			pEntity->addModel(pModel);
			pEntity->addComponent<CCollision>();
			return true;
		}
		else
		{
			return false;
		}
	}

	bool CEntityMgr::loadLightEntity(CActorEntity* pEntity, std::string szFile)
	{
		return true;
	}

	bool CEntityMgr::loadSkyEntity(CActorEntity* pEntity, std::string szModel)
	{
		auto pModel = dynamic_cast<CModel*>(GEngine->createResource(szModel, Model));
		if (nullptr != pModel)
		{
			pEntity->addModel(pModel, 0);
			pEntity->removeComponent<CCollision>();
			return true;
		}
		else
		{
			return false;
		}
	}

	bool CEntityMgr::loadSkyBoxEntity(CActorEntity* pEntity, std::string szModel)
	{
#if 0
		auto pConfig = GEngine->getWindowConfigPtr();
		auto pModel = GEngine->createBillBoardModel(R"(resource\Material\skyBox.ma)", pConfig->nWidth, pConfig->nHeight);
		if (nullptr != pModel)
		{
			pEntity->addModel(pModel, 0);
			auto pModelInstance = pEntity->getModelInstance();
			auto pMatInstance = pModelInstance->getMaterialInstance();
			pMatInstance->setRenderOrder(256);
			pEntity->removeComponent<CCollision>();
			return true;
		}
		else
		{
			return false;
		}

#else
		auto pModel = dynamic_cast<CModel*>(GEngine->createResource(szModel, Model));
		if (nullptr != pModel)
		{
			pEntity->addModel(pModel, 0);
			pEntity->removeComponent<CCollision>();
			return true;
		}
		else
		{
			return false;
		}
#endif
	}

	bool CEntityMgr::loadTerrainEntity(CEntity* pEntity, int builderType, std::string szSizeInfo)
	{
		switch (builderType)
		{
		case 1:
		{}
			break;
		default:
			break;
		}
		return true;
	}

	bool CEntityMgr::loadOceanEntity(CActorEntity* pEntity, std::string szModel)
	{
		auto pModel = dynamic_cast<CModel*>(GEngine->createResource(szModel, Model));
		if (nullptr != pModel)
		{
			pEntity->addModel(pModel, 0);
			pEntity->removeComponent<CCollision>();
			return true;
		}
		else
		{
			return false;
		}
	}

}