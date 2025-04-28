#include "ModelInstance.h"
#include "SkeletonAniComponent.h"

namespace WL
{

	CModelInstance::CModelInstance(CModel* pModel)
	{
		bindModel(pModel);
	}

	CModelInstance::~CModelInstance()
	{
		clearModel();

	}

	void CModelInstance::clearModel()
	{
		WL_DECREASE(mpModel);
		for (auto item : mMeshInstances)
		{
			WL_DELETE(item.pMaterialInstance, Instance);
			WL_DELETE(item.pMeshInstance, Instance);
			SafeDelete(item.pAnimation);
		}
		mMeshInstances.clear();
	}

	CInstance* CModelInstance::clone()
	{
		CModelInstance* pClone = WL_NEW(CModelInstance, Instance)(mpModel);
		pClone->setLocalPosition(getLocalPosition());
		pClone->setLocalScale(getLocalScale());
		pClone->setLocalRotation(getLocalRotation());

		pClone->setPosition(getPosition());
		pClone->setScale(getScale());
		pClone->setRotation(getRotation());
		pClone->mMeshInstances = mMeshInstances;
		return pClone;
	}

	void CModelInstance::operator=(const CModelInstance& instance)
	{
		if (this != &instance)
		{
			WL_DECREASE(mpModel);
			mpModel = instance.mpModel;
			if (nullptr != instance.mpModel)
			{
				mpModel->addRef();
			}
			
			setLocalPosition(instance.getLocalPosition());
			setLocalScale(instance.getLocalScale());
			setLocalRotation(instance.getLocalRotation());

			setPosition(instance.getPosition());
			setScale(instance.getScale());
			setRotation(instance.getRotation());
			mMeshInstances = instance.mMeshInstances;
		}
	}

	void CModelInstance::setPosition(const Vec3F& vec)
	{
		auto&& v = vec + mLocalPosition;
		CTranformComponet::setPosition(v);
		CTranformComponet::updateMatrix();
		for (auto& item : mMeshInstances)
		{
			auto& position = item.pMeshInstance->getLocalPosition();
			auto& modelPosition = getPosition();
			item.pMeshInstance->setPosition(modelPosition + position);
		}
	}

	void CModelInstance::setScale(const Vec3F& vec)
	{
		auto&& v = vec * mLocalScale;
		CTranformComponet::setScale(v);
		CTranformComponet::updateMatrix();
		for (auto& item : mMeshInstances)
		{
			auto& modelScale = getScale();
			item.pMeshInstance->setScale(modelScale);
		}
	}

	void CModelInstance::setRotation(const Vec3F& vec)
	{
		auto&& v = vec + mLocalRotation;
		CTranformComponet::setRotation(v);
		CTranformComponet::updateMatrix();
		for (auto& item : mMeshInstances)
		{
			auto& rotation = item.pMeshInstance->getLocalRotation();
			auto& modelRotation = getRotation();
			item.pMeshInstance->setRotation(modelRotation + rotation);
		}
	}

	void CModelInstance::setLocalPosition(const Vec3F& vec)
	{
		mLocalPosition = vec;
		CTranformComponet::setPosition(vec);
		CTranformComponet::updateMatrix();
	}

	void CModelInstance::setLocalScale(const Vec3F& vec)
	{
		mLocalScale = vec;
		CTranformComponet::setScale(vec);
		CTranformComponet::updateMatrix();
	}

	void CModelInstance::setLocalRotation(const Vec3F& vec)
	{
		mLocalRotation = vec;
		CTranformComponet::setRotation(vec);
		CTranformComponet::updateMatrix();
	}

	const Vec3F& CModelInstance::getLocalPosition() const
	{
		return mLocalPosition;
	}

	const Vec3F& CModelInstance::getLocalScale() const
	{
		return mLocalScale;
	}

	const Vec3F& CModelInstance::getLocalRotation() const
	{
		return mLocalRotation;
	}


	void CModelInstance::bindModel(CModel* pModel)
	{
		if (mpModel != pModel)
		{
			WL_DECREASE(mpModel);
			mpModel = pModel;
			if (nullptr != pModel)
			{
				mpModel->addRef();
				mAABB = mpModel->getBoundBox();

				setLocalPosition(pModel->getPosition());
				setLocalScale(pModel->getScale());
				setLocalRotation(pModel->getRotation());
				
				auto meshs = mpModel->getAllMeshInfo();
				for (auto& item : meshs)
				{
					MeshInstanceInfo info;
					info.name = item.szName;
					info.pMeshInstance = WL_NEW(CMeshInstance, Instance)(item.pMesh);
					info.pMaterialInstance = WL_NEW(CMaterialInstance, Instance)(item.pMaterial);
					for (int i = 0; i < (int)item.textures.size(); ++i)
					{
						info.pMaterialInstance->bindTexture((TextureUseType)i, item.textures[i]);
					}
					info.pMaterialInstance->bindTexturaArray(&item.textureArray);
					mMeshInstances.emplace_back(info);
				}
			}
		}
	}

	void CModelInstance::rebindModel(CModel* pModel)
	{
		clearModel();
		bindModel(pModel);
	}

	std::vector<MeshInstanceInfo>& CModelInstance::getAllMeshInfo()
	{
		return mMeshInstances;
	}

	CModel* CModelInstance::getModel() const
	{
		return mpModel;
	}

	CMaterialInstance* CModelInstance::getMaterialInstance(int nSubMesh /*= 0*/) const
	{
		CMaterialInstance* pMaterialInstance = nullptr;
		int nSize = mMeshInstances.size();
		if(nSize > 0 && nSubMesh < nSize)
		{
			if (nSubMesh < nSize)
			{
				pMaterialInstance = mMeshInstances[nSubMesh].pMaterialInstance;
			}
		}
		return pMaterialInstance;
	}

	bool CModelInstance::isVisual() const 
	{
		return mbVisual;
	}

	void CModelInstance::setVisual(bool bValue)
	{
		mbVisual = bValue;
	}

	void CModelInstance::addMesh(CMesh* pMesh)
	{
		if (nullptr != mpModel)
		{
			auto pModel = mpModel;
			mpModel->addMesh(pMesh);
			pModel->addRef();
			rebindModel(mpModel);
			WL_DECREASE(pModel);
		}
	}

}