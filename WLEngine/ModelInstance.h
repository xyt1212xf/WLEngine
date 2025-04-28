#pragma once
#include "Instance.h"
#include "Transform.h"
#include "Model.h"
#include "MeshInstance.h"

namespace WL
{
	class CModelInstance : public CInstance,
						   public CTranformComponet
	{
	public:
		CModelInstance(CModel* pModel);
		virtual ~CModelInstance();

		void clearModel();

		virtual CInstance* clone();
		virtual void setPosition(const Vec3F& vec)final;
		virtual void setScale(const Vec3F& vec)final;
		virtual void setRotation(const Vec3F& vec) final;
		virtual void bindModel(CModel* pModel);
		virtual void rebindModel(CModel* pModel);
		virtual void addMesh(CMesh* pMesh);

		void operator = (const CModelInstance& instance);
		void setLocalPosition(const Vec3F& vec);
		void setLocalScale(const Vec3F& vec);
		void setLocalRotation(const Vec3F& vec);

		const Vec3F& getLocalPosition() const;
		const Vec3F& getLocalScale() const;
		const Vec3F& getLocalRotation() const;


		std::vector<MeshInstanceInfo>& getAllMeshInfo();
		CModel* getModel() const;
		CMaterialInstance* getMaterialInstance(int nSubMesh = 0)const;

		bool isVisual() const;
		void setVisual(bool bValue);

	protected:
		Vec3F	mLocalPosition = { 0, 0, 0 };
		Vec3F	mLocalScale = { 1, 1, 1 };
		Vec3F	mLocalRotation = { 0, 0, 0 };
		CModel* mpModel = nullptr;
		std::vector<MeshInstanceInfo>	mMeshInstances;
		bool    mbVisual = true;
	};
}