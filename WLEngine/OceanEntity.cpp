#include "OceanEntity.h"
#include "WLEngine.h"

namespace WL
{
	// Subdivision thredshold. Any quad covers more pixels than this value needs to be subdivided.
	COceanEntity::COceanEntity()
	{
		mType = Ocean;
	}

	COceanEntity::~COceanEntity()
	{
		mpModelInstance = nullptr;
	}

	bool COceanEntity::initialise()
	{
		mSimulator.initialise();
		mpModelInstance->addMesh(dynamic_cast<CMesh*>(mSimulator.mpMesh));
		return true;
	}

	void COceanEntity::addModel(CModel* pModel, INT32 nSlot)
	{
		pModel->addRef();
		COceanModelInstance* pInstance = WL_NEW(COceanModelInstance, Instance)(pModel);
		mModelInstanceMap[nSlot] = pInstance;
		mpModelInstance = pInstance;
		mAABB.unionBoundBox(pInstance->getBoundBox());
		mSimulator.mpModelInstance = pInstance;
		mSimulator.setMaterial(pModel->getMaterial());
		WL_DECREASE(pModel);
	}

	void COceanEntity::draw(UINT32 nTime)
	{
		mSimulator.updateNodes(nTime);
	}

	void COceanEntity::_tick(UINT32 dt)
	{
	}

	void COceanEntity::_updateDisplacementMap(float time)
	{

	}

}