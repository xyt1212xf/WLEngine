#include "CameraEntity.h"
#include "MathLib.h"
#include "WLEngine.h"
#include "Scene.h"
namespace WL
{

	CFrustum::CFrustum(SMatrix44* pViewMatrix, SMatrix44* pProjectMatrix)
	{
		mpViewMatrix = pViewMatrix;
		mpProjectMatrix = pProjectMatrix;
		mpDeviceConfig = GEngine->getWindowConfigPtr();
	}

	void CFrustum::update()
	{

	}

	// Check if the point is inside all six planes of the view frustum.
	bool CFrustum::checkPoint(const Vec3F& pt)
	{
		for (INT32 i = 0; i < 6; i++)
		{
			if (Math::planeDotCoord(m_planes[i], pt) < 0.0f)
			{
				return false;
			}
		}

		return true;
	}

	// Check if any one point of the cube is in the view frustum.
	bool CFrustum::checkCube(const Vec3F& pCenter, float radius)
	{
		for (INT32 i = 0; i < 6; i++)
		{
			if (Math::planeDotCoord(m_planes[i], Vec3F((pCenter.x - radius), (pCenter.y - radius), (pCenter.z - radius))) >= 0.0f)
			{
				continue;
			}

			if (Math::planeDotCoord(m_planes[i], Vec3F((pCenter.x + radius), (pCenter.y - radius), (pCenter.z - radius))) >= 0.0f)
			{
				continue;
			}

			if (Math::planeDotCoord(m_planes[i], Vec3F((pCenter.x - radius), (pCenter.y + radius), (pCenter.z - radius))) >= 0.0f)
			{
				continue;
			}

			if (Math::planeDotCoord(m_planes[i], Vec3F((pCenter.x + radius), (pCenter.y + radius), (pCenter.z - radius))) >= 0.0f)
			{
				continue;
			}

			if (Math::planeDotCoord(m_planes[i], Vec3F((pCenter.x - radius), (pCenter.y - radius), (pCenter.z + radius))) >= 0.0f)
			{
				continue;
			}

			if (Math::planeDotCoord(m_planes[i], Vec3F((pCenter.x + radius), (pCenter.y - radius), (pCenter.z + radius))) >= 0.0f)
			{
				continue;
			}

			if (Math::planeDotCoord(m_planes[i], Vec3F((pCenter.x - radius), (pCenter.y + radius), (pCenter.z + radius))) >= 0.0f)
			{
				continue;
			}

			if (Math::planeDotCoord(m_planes[i], Vec3F((pCenter.x + radius), (pCenter.y + radius), (pCenter.z + radius))) >= 0.0f)
			{
				continue;
			}

			return false;
		}

		return true;
	}

	// Check if the radius of the sphere is inside the view frustum.
	bool CFrustum::checkSphere(const Vec3F& pCenter, float radius)
	{
		for (INT32 i = 0; i < 6; i++)
		{
			if (Math::planeDotCoord(m_planes[i], pCenter) < -radius)
			{
				return false;
			}
		}

		return true;
	}

	// Check if any of the 6 planes of the rectangle are inside the view frustum.
	bool CFrustum::checkRectangle(const Vec3F& pCenter, const Vec3F& radius)
	{
		for (INT32 i = 0; i < 6; i++)
		{
			if (Math::planeDotCoord(m_planes[i], Vec3F((pCenter.x - radius.x), (pCenter.y - radius.y), (pCenter.z - radius.z))) >= 0.0f)
			{
				continue;
			}

			if (Math::planeDotCoord(m_planes[i], Vec3F((pCenter.x + radius.x), (pCenter.y - radius.y), (pCenter.z - radius.z))) >= 0.0f)
			{
				continue;
			}

			if (Math::planeDotCoord(m_planes[i], Vec3F((pCenter.x - radius.x), (pCenter.y + radius.y), (pCenter.z - radius.z))) >= 0.0f)
			{
				continue;
			}

			if (Math::planeDotCoord(m_planes[i], Vec3F((pCenter.x - radius.x), (pCenter.y - radius.y), (pCenter.z + radius.z))) >= 0.0f)
			{
				continue;
			}

			if (Math::planeDotCoord(m_planes[i], Vec3F((pCenter.x + radius.x), (pCenter.y + radius.y), (pCenter.z - radius.z))) >= 0.0f)
			{
				continue;
			}

			if (Math::planeDotCoord(m_planes[i], Vec3F((pCenter.x + radius.x), (pCenter.y - radius.y), (pCenter.z + radius.z))) >= 0.0f)
			{
				continue;
			}

			if (Math::planeDotCoord(m_planes[i], Vec3F((pCenter.x - radius.x), (pCenter.y + radius.y), (pCenter.z + radius.z))) >= 0.0f)
			{
				continue;
			}

			if (Math::planeDotCoord(m_planes[i], Vec3F((pCenter.x + radius.x), (pCenter.y + radius.y), (pCenter.z + radius.z))) >= 0.0f)
			{
				continue;
			}

			return false;
		}

		return true;
	}

	//////////////////////////////////////////////////////////////////////////
	 
	
	CCameraEntity::CCameraEntity()
	{
		mType = Camera;
		mpFrustum = WL_NEW(CFrustum, Frustum)(&mViewMatrix, &mProjectMatrix);
	}

	CCameraEntity::~CCameraEntity()
	{
		for (auto item : mDrawEntities)
		{
			WL_DECREASE(item);
		}
		mDrawEntities.clear();
	}

	void CCameraEntity::setEye(const Vec3F& vec)
	{
		mEye = vec;
		buildViewMatrix();
	}

	void CCameraEntity::setLookAt(const Vec3F& vec)
	{
		mLookAt = vec;
		buildViewMatrix();
	}

	void CCameraEntity::setFov(float fFov)
	{
		mFov = fFov;
		buildPerspectiveFovMatrix(fFov, mWndWidth, mWndHeight, mNearPlane, mFarPlane);
	}

	void CCameraEntity::setAspect(float fWndWidth, float fWndHeight )
	{
		mAspect = fWndWidth / fWndHeight;
		mWndWidth = fWndWidth;
		mWndHeight = fWndHeight;
		buildPerspectiveFovMatrix(mFov, mWndWidth, mWndHeight, mNearPlane, mFarPlane);
	}

	void CCameraEntity::setNearPlane(float fPlane)
	{
		mNearPlane = fPlane;
		buildPerspectiveFovMatrix(mFov, mWndWidth, mWndHeight, mNearPlane, mFarPlane);
	}

	void CCameraEntity::setFarPlane(float fPlane)
	{
		mFarPlane = fPlane;
		buildPerspectiveFovMatrix(mFov, mWndWidth, mWndHeight, mNearPlane, mFarPlane);
	}

	const Vec3F& CCameraEntity::getEye() const
	{
		return mEye;
	}

	const Vec3F& CCameraEntity::getLookAt() const
	{
		return mLookAt;
	}

	float CCameraEntity::getFov()
	{
		return mFov;
	}


	float CCameraEntity::getAspect()
	{
		return mAspect;
	}

	float CCameraEntity::getNearPlane()
	{
		return mNearPlane;
	}

	float CCameraEntity::getFarPlane()
	{
		return mFarPlane;
	}

	CameraType CCameraEntity::getCameraType()
	{
		return mCameraType;
	}

	void CCameraEntity::setCameraType(CameraType type)
	{
		mCameraType = type;
	}

	float CCameraEntity::getMoveSpeed() const 
	{
		return mMoveSpeed;
	}

	void CCameraEntity::setMoveSpeed(float fSpeed)
	{
		mMoveSpeed = fSpeed;
	}

	void CCameraEntity::buildViewMatrix()
	{	
		Math::buildMatrixLookAtLH(&mViewMatrix, &mEye, &mLookAt, &mUp);
		Math::matrixTranspose(&mTransposeViewMatrix, &mViewMatrix);
		GEngine->setViewMatrxi(&mViewMatrix);
		GEngine->setTransposeViewMatrxi(&mTransposeViewMatrix);
		_makeViewProjectMatrix();
		_updateDirection();
	}

	void CCameraEntity::buildPerspectiveFovMatrix(float fovy, float wndWidth, float wndHeight, float fNear, float fFar)
	{
		mWndWidth = wndWidth;
		mWndHeight = wndHeight;
		float Aspect = wndWidth / wndHeight;
		mFov = fovy;
		mAspect = Aspect;
		mNearPlane = fNear;
		mFarPlane = fFar;
		mViewSize.x = (INT32)wndWidth;
		mViewSize.y = (INT32)wndHeight;
		Math::buildMatrixPerspectiveFovLH(&mProjectMatrix, fovy, Aspect, fNear, fFar);
		Math::matrixTranspose(&mTransposeProjectMatrix, &mProjectMatrix);
		GEngine->setProjectMatrix(&mProjectMatrix);
		GEngine->setTransposeProjectMatrix(&mTransposeProjectMatrix);

		_makeViewProjectMatrix();
	}

	SMatrix44* CCameraEntity::getViewMatrixPtr() const
	{
		return &mViewMatrix;
	}

	SMatrix44* CCameraEntity::getProjectMatrixPtr() const
	{
		return &mProjectMatrix;
	}

	SMatrix44* CCameraEntity::getViewMatrixTransposePtr() const
	{
		return &mTransposeViewMatrix;
	}

	SMatrix44* CCameraEntity::getProjectMatrixTransposePtr() const
	{
		return &mTransposeProjectMatrix;
	}

	SMatrix44 CCameraEntity::getViewMatrix() const
	{
		return mViewMatrix;
	}

	SMatrix44 CCameraEntity::getProjectMatrix() const
	{
		return mProjectMatrix;
	}

	SMatrix44 CCameraEntity::getViewMatrixTranspose() const
	{
		return mTransposeViewMatrix;
	}

	SMatrix44 CCameraEntity::getProjectMatrixTranspose() const
	{
		return mTransposeProjectMatrix;
	}

	std::vector<CActorEntity*>& CCameraEntity::getDrawEntities()
	{
		return	mDrawEntities;
	}

	void CCameraEntity::addDrawEntities(std::list<CActorEntity*>& pIn, UINT32 nTime)
	{
		for (auto item : pIn)
		{
			if (item->isVisible())
			{
				auto& aabb = item->getBoundBox();
				if (mpFrustum->checkRectangle(aabb.getCenter(), aabb.getExtent()))
				{
					mDrawEntities.emplace_back(item);
					item->addRef();
					item->draw(nTime);
				}
			}
		}
	}

	void CCameraEntity::clearDrawEntities()
	{
		for (auto item : mDrawEntities)
		{
			WL_DECREASE(item);
		}
		mDrawEntities.clear();
	}

	Vec3F CCameraEntity::vec3ToScreenSpace(const Vec3F& worldPos) const
	{
		Vec4F tempPos = { worldPos.x, worldPos.y, worldPos.z, 1 };
		tempPos = Math::matrixMulVec4(GEngine->getMainCameraEntity()->getViewMatrix(), tempPos);
		tempPos = Math::matrixMulVec4(GEngine->getMainCameraEntity()->getProjectMatrix(), tempPos);
		const auto& size = GEngine->getMainCameraEntity()->getViewSize();

		return Vec3F(size.x + (1.0f + tempPos.x) * size.x * 0.5f,
					 size.y + (1.0f - tempPos.y) * size.y* 0.5f,
					 tempPos.z);
	}

	const Vec2I& CCameraEntity::getViewSize() const
	{
		return mViewSize;
	}

	Vec2I CCameraEntity::changeToScreenCoordinate(const Vec3F& vPos)
	{
		Vec2I p;
		auto v = Math::matrixMulVec4(mViewProjectMatrix, Vec4F(vPos.x, vPos.y, vPos.z, 1));
		p.x = (INT32)(((v.x / v.w) * 0.5f + 0.5f) * mViewSize.x);
		p.y = (INT32)((1.0f - ((v.y / v.w) * 0.5f + 0.5f)) * mViewSize.y);
		return p;
	}

	void CCameraEntity::addDrawEntity(CActorEntity* pActor)
	{
		if (pActor->isVisible())
		{
			auto& aabb = pActor->getBoundBox();
			if (mpFrustum->checkRectangle(aabb.getCenter(), aabb.getExtent()))
			{
				mDrawEntities.emplace_back(pActor);
				pActor->addRef();
			}
		}

	}

	void CCameraEntity::_tick(UINT32 dt)
	{
	}

	void CCameraEntity::_makeViewProjectMatrix()
	{
		mViewProjectMatrix = mViewMatrix * mProjectMatrix;
		Math::matrixTranspose(&mTransposeViewProjectMatrix, &mViewProjectMatrix);
		GEngine->setViewProjectMatrix(&mViewProjectMatrix);
		GEngine->setTransposeViewProjectMatrix(&mTransposeViewProjectMatrix);
	}

	void CCameraEntity::_updateDirection()
	{
		mFrontDirection = mLookAt - mEye;
		mFrontDirection.normalize();
		mFrontDirection *= mMoveSpeed;
		mLeftDirection = mFrontDirection;
		Math::vec3RotationY(&mLeftDirection,   Math::angleToRadian(90.0f)); 
	}

}