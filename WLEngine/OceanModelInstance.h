#pragma once
#include "ModelInstance.h"
namespace WL
{
	class COceanModelInstance : public CModelInstance
	{
	public:
		COceanModelInstance(CModel* pModel);
		virtual ~COceanModelInstance();
		virtual void bindModel(CModel* pModel) final;
		virtual void rebindModel(CModel* pModel)final;
		void addMeshInstance(const MeshInstanceInfo& meshInstance);
		void clearMeshs();
	};
}