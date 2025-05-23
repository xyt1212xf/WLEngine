#pragma once
#include "Entity.h"
#include "Vector.h"
#include "Matrix.h"
#include "Plane.h"
#include "Transform.h"
#include "ActorEntity.h"

namespace WL
{
	enum CameraType
	{
		Free,
		Third,
		First,
		Custom,
		MaxCameraType,
	};
	class CFrustum
	{
		friend class CCameraEntity;
	private:
		CFrustum(SMatrix44* pViewMatrix, SMatrix44* pProjectMatrix);
		void update();
		bool checkPoint(const Vec3F& pt);
		bool checkCube(const Vec3F& pCenter, float radius);
		bool checkSphere(const Vec3F& pCenter, float radius);
		bool checkRectangle(const Vec3F& pCenter, const Vec3F& radius);

	private:
		SPlane m_planes[6];
		SMatrix44* mpViewMatrix = nullptr;
		SMatrix44* mpProjectMatrix = nullptr;
		SWindowConfig* mpDeviceConfig = nullptr;
	};

	class CCameraEntity : public CEntity,
						  public CTranformComponet
	{
		friend class CCameraController;

	public:
		CCameraEntity();
		virtual ~CCameraEntity();
		void setEye( const Vec3F& vec );
		void setLookAt(const Vec3F& vec);
		void setFov(float fFov);
		void setAspect(float fWndWidth, float fWndHeight);
		void setNearPlane(float fPlane);
		void setFarPlane(float fPlane);

		const Vec3F& getEye()const;
		const Vec3F& getLookAt()const;
		float getFov();
		float getAspect();
		float getNearPlane();
		float getFarPlane();

		CameraType getCameraType();
		void setCameraType(CameraType type);

		float getMoveSpeed() const;
		void setMoveSpeed(float fSpeed);

		void buildViewMatrix();
		void buildPerspectiveFovMatrix(float fovy, float wndWidth, float wndHeight, float fNear, float fFar);
		SMatrix44* getViewMatrixPtr() const;
		SMatrix44* getProjectMatrixPtr() const;
		SMatrix44* getViewMatrixTransposePtr() const;
		SMatrix44* getProjectMatrixTransposePtr() const;

		SMatrix44 getViewMatrix()const;
		SMatrix44 getProjectMatrix()const;
		SMatrix44 getViewMatrixTranspose()const;
		SMatrix44 getProjectMatrixTranspose()const;
		//CRay*	  getRayFromScreen(int nX, int nY);

		std::vector<CActorEntity*>& getDrawEntities();
		void addDrawEntity(CActorEntity* pActor);
		void addDrawEntities(std::list<CActorEntity*>& pIn, UINT32 nTime);
		void clearDrawEntities();

		Vec3F vec3ToScreenSpace(const Vec3F& worldPos) const;
		const Vec2I& getViewSize() const;
		Vec2I changeToScreenCoordinate(const Vec3F& vPos);
		
	private:
		virtual void _tick(UINT32 dt) final;
		void _makeViewProjectMatrix();
		void _updateDirection();

	private:
		Vec2I		mViewSize = { 0,0 };
		Vec3F		mEye = {0,0,0};
		Vec3F		mLookAt = { 0,0,0 };
		Vec3F		mUp = { 0, 1, 0 };
		Vec3F		mFrontDirection = { 0,0,0 };
		Vec3F		mLeftDirection = { 0,0,0 };
		mutable SMatrix44	mViewMatrix;
		mutable SMatrix44	mProjectMatrix;
		SMatrix44	mViewProjectMatrix;
		mutable SMatrix44	mTransposeViewMatrix;
		mutable SMatrix44	mTransposeProjectMatrix;
		SMatrix44	mTransposeViewProjectMatrix;
		CameraType	mCameraType = Free;
		CFrustum*	mpFrustum = nullptr;
		std::vector<CActorEntity*>	mDrawEntities;
		float		mFov = 0;
		float		mAspect = 0;
		float		mNearPlane = 0;
		float		mFarPlane = 0;
		float		mMoveSpeed = 0.01f;
		float		mWndWidth = 1.0f;
		float		mWndHeight = 1.0f;
	};
}