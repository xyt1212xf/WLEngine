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
		CFrustum(Matrix44* pViewMatrix, Matrix44* pProjectMatrix);
		void update();
		bool checkPoint(const Vec3F& pt);
		bool checkCube(const Vec3F& pCenter, float radius);
		bool checkSphere(const Vec3F& pCenter, float radius);
		bool checkRectangle(const Vec3F& pCenter, const Vec3F& radius);

	private:
		Plane m_planes[6];
		Matrix44* mpViewMatrix = nullptr;
		Matrix44* mpProjectMatrix = nullptr;
		WindowConfig* mpDeviceConfig = nullptr;
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
		Matrix44* getViewMatrixPtr() const;
		Matrix44* getProjectMatrixPtr() const;
		Matrix44* getViewMatrixTransposePtr() const;
		Matrix44* getProjectMatrixTransposePtr() const;

		Matrix44 getViewMatrix()const;
		Matrix44 getProjectMatrix()const;
		Matrix44 getViewMatrixTranspose()const;
		Matrix44 getProjectMatrixTranspose()const;
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
		mutable Matrix44	mViewMatrix;
		mutable Matrix44	mProjectMatrix;
		Matrix44	mViewProjectMatrix;
		mutable Matrix44	mTransposeViewMatrix;
		mutable Matrix44	mTransposeProjectMatrix;
		Matrix44	mTransposeViewProjectMatrix;
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