#include "OceanModelInstance.h"
namespace WL
{
	COceanModelInstance::COceanModelInstance(CModel* pModel)
	: CModelInstance(pModel)
	{

	}

	COceanModelInstance::~COceanModelInstance()
	{

	}

	void COceanModelInstance::bindModel(CModel* pModel)
	{
		CModelInstance::bindModel(pModel);
	}

	void COceanModelInstance::rebindModel(CModel* pModel)
	{

	}

	void COceanModelInstance::addMeshInstance(const SMeshInstanceInfo& meshInstance)
	{
		mMeshInstances.emplace_back(meshInstance);
	}

	void COceanModelInstance::clearMeshs()
	{
		mMeshInstances.clear();
	}

}