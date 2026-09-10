#include "MeshInstance.h"
#include "WLEngine.h"

namespace WL
{
	CMeshInstance::CMeshInstance(CMesh* pMesh)
	{
		bindMesh(pMesh);
		setViewMT(GEngine->getViewMatrix());
		setTransposeViewMT(GEngine->getTransposeViewMatrix());

		setProjectMT(GEngine->getProjectMatrix());
		setTransposeProjectMT(GEngine->getTransposeProjectMatrix());
	}

	CMeshInstance::~CMeshInstance()
	{
		WL_DECREASE(mpMesh);
	}

	void CMeshInstance::bindMesh(CMesh* pMesh)
	{
		if (mpMesh != pMesh)
		{
			WL_DECREASE(mpMesh);
			mpMesh = pMesh;
			if (nullptr != pMesh)
			{
				mpMesh->addRef();
				mAABB = mpMesh->getBoundBox();
				setPosition(pMesh->getPosition());
				setScale(pMesh->getScale());
				setRotation(pMesh->getRotation());

				setLocalPosition(pMesh->getPosition());
				setLocalScale(pMesh->getScale());
				setLocalRotation(pMesh->getRotation());
				
			}
		}
	}

	CGpuBuffer* CMeshInstance::getVertexBuffer() const
	{
		return mpMesh->getVertexBuffer();
	}

	CGpuBuffer* CMeshInstance::getIndexBuffer() const
	{
		return mpMesh->getIndexBuffer();
	}

	UINT32 CMeshInstance::getVertexTypeSize(int nLod /*= -1*/) const
	{
		if (nullptr != mpMesh)
		{
			return mpMesh->getVertexTypeSize(nLod);
		}
		else
		{
			return 0;
		}

	}

	UINT32 CMeshInstance::getIndexCount(int nLod /*= -1*/) const
	{
		if (mIndexCount != 0)
		{
			return mIndexCount;
		}
		else
		{
			if (nullptr != mpMesh)
			{
				return mpMesh->getIndexCount(nLod);
			}
			else
			{
				return 0;
			}
		}
	}


	UINT32 CMeshInstance::getIndeiceType()
	{
		if (nullptr != mpMesh)
		{
			return mpMesh->getIndeiceType();
		}
		else
		{
			return 0;
		}
	}

	UINT32 CMeshInstance::getVertexOffset()
	{
		if (nullptr != mpMesh)
		{
			return mpMesh->getVertexOffset();
		}
		else
		{
			return 0;
		}
	}

	CMesh* CMeshInstance::getMesh() const
	{
		return mpMesh;
	}


	void CMeshInstance::setIndexCount(UINT32 nCount)
	{
		mIndexCount = nCount;
	}

	void CMeshInstance::setLocalPosition(const Vec3F& position)
	{
		mLocalPosition = position;
	}

	void CMeshInstance::setLocalScale(const Vec3F& scale)
	{
		mLocalScale = scale;
	}

	void CMeshInstance::setLocalRotation(const Vec3F& rotation)
	{
		mLocalRotation = rotation;
	}

	const Vec3F& CMeshInstance::getLocalPosition() const
	{
		return mLocalPosition;
	}

	const Vec3F& CMeshInstance::getLocalScale() const
	{
		return mLocalScale;
	}

	const Vec3F& CMeshInstance::getLocalRotation() const
	{
		return mLocalRotation;
	}

	void CMeshInstance::setPosition(const Vec3F& vec)
	{
		auto v = vec + mLocalPosition;
		CTranformComponet::setPosition(v);
		CTranformComponet::updateMatrix();
	}

	void CMeshInstance::setScale(const Vec3F& vec)
	{
		auto v = vec * mLocalScale;
		CTranformComponet::setScale(v);
		CTranformComponet::updateMatrix();
	}

	void CMeshInstance::setRotation(const Vec3F& vec)
	{
		auto v = vec * mLocalRotation;
		CTranformComponet::setRotation(v);
		CTranformComponet::updateMatrix();
	}

	void CMeshInstance::setViewMT(SMatrix44* pMT)
	{
		mpViewMT = pMT;
	}

	void CMeshInstance::setTransposeViewMT(SMatrix44* pMT)
	{
		mpTransposeViewMT = pMT;
	}

	void CMeshInstance::setProjectMT(SMatrix44* pMT)
	{
		mpProjectMT = pMT;
	}

	void CMeshInstance::setTransposeProjectMT(SMatrix44* pMT)
	{
		mpTransposeProjectMT = pMT;
		if (nullptr != mpTransposeViewMT && nullptr != mpTransposeProjectMT)
		{
			mTransposeViewProjectMT = (*mpTransposeViewMT) * (*mpTransposeProjectMT);
		}
	}

	SMatrix44& CMeshInstance::getViewMT() const
	{
		return *mpViewMT;
	}

	SMatrix44& CMeshInstance::getTransposeViewMT()const
	{
		return *mpTransposeViewMT;
	}

	SMatrix44& CMeshInstance::getProjectMT()const
	{
		return *mpProjectMT;
	}

	SMatrix44& CMeshInstance::getTransposeProjectMT()const
	{
		return *mpTransposeProjectMT;
	}

	SMatrix44& CMeshInstance::getTransposeViweProjectMatrix() const
	{
		return mTransposeViewProjectMT;
	}

	void CMeshInstance::setStartIndexLocation(UINT32 nStart)
	{
		mStartIndex = nStart;
	}

	UINT32 CMeshInstance::getStartIndexLocation() const
	{
		return mStartIndex;
	}

	void CMeshInstance::setBaseVertexLocation(UINT32 nStart)
	{
		mBaseStartIndex = nStart;
	}

	UINT32 CMeshInstance::getBaseVertexLocation() const
	{
		return mBaseStartIndex;
	}

	void CMeshInstance::operator=(const CMeshInstance& instance)
	{
		if (this != &instance)
		{
			mpMesh = instance.mpMesh;
			mpMesh->addRef();
		}
	}

	//////////////////////////////////////////////////////////////////////////	
	void SMeshInstanceInfo::operator=(const SMeshInstanceInfo& instance)
	{
		name = instance.name;
		pMeshInstance = WL_NEW(CMeshInstance, Instance)(instance.pMeshInstance->getMesh());
		pMaterialInstance = WL_NEW(CMaterialInstance, Instance)(instance.pMaterialInstance->getMaterial());
		//pAnimation = new CAnimationComponet;
	}
}

