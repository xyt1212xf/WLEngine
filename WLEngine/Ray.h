#pragma once
#include "Vector.h"
#include "AABBox.h"
#include "Sphere.h"

namespace WL
{
	class CActorEntity;
	class CCameraEntity;
	class CRay
	{
	public:
		explicit CRay(CCameraEntity* pCamera);
		~CRay();
		void setScreenMouseSpace(int nMouseX, int nMouseY);
		void setDirection(const Vec3F& vec);
		void setOrigin(const Vec3F& vec);
		void setRange(float fRange);
		bool intersectAABB(const CAABBox& aabb);
		bool intersect(const CSphere& sphere);
		bool intersectTriangle(const Vec3F* v0, const Vec3F* v1, const Vec3F* v2);
		bool intersectActorEntity(CActorEntity* pActorEntity);
		CActorEntity* intersectActorEntities(CActorEntity* pActorEntity, int nCount);

	public:
		Intersection mResult;
		Vec3F  mOriginPosition = { 0, 0, 0 };
		Vec3F  mDirection = { 0, 0, 0 };

	private:
		CCameraEntity* mpCameraEntity = nullptr;
		Vec3F  mPickScreenPos = { 0, 0, 0 };
		float mRange = 0;
	};
}