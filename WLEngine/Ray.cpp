#include "Ray.h"
#include "MathLib.h"
#include "Transform.h"
#include "CameraEntity.h"
#include "ActorEntity.h"

namespace WL
{

	CRay::CRay(CCameraEntity* pCamera)
	: mpCameraEntity(pCamera)
	{
		pCamera->addRef();
		mResult.bPick = false;
	}

	CRay::~CRay()
	{
		WL_DECREASE(mpCameraEntity);
	}

	void CRay::setScreenMouseSpace(int nMouseX, int nMouseY)
	{
		if (nullptr != mpCameraEntity)
		{
			// Move the mouse cursor coordinates into the -1 to +1 range.
			const auto& projectMatrix = mpCameraEntity->getProjectMatrix();
			const auto& viewSize = mpCameraEntity->getViewSize();
			mPickScreenPos.x = ((2.0f * nMouseX / viewSize.x) - 1.0f) / projectMatrix._11;
			mPickScreenPos.y = -((2.0f * nMouseY / viewSize.y) - 1.0f) / projectMatrix._22;
			mPickScreenPos.z = 1.0f;

			const Matrix44& matView = *mpCameraEntity->getViewMatrixPtr();
			// Get the inverse view matrix
			Matrix44 m;
			Math::matrixInverse(&m, &matView);

			mDirection.x = mPickScreenPos.x * m._11 + mPickScreenPos.y * m._21 + mPickScreenPos.z * m._31;
			mDirection.y = mPickScreenPos.x * m._12 + mPickScreenPos.y * m._22 + mPickScreenPos.z * m._32;
			mDirection.z = mPickScreenPos.x * m._13 + mPickScreenPos.y * m._23 + mPickScreenPos.z * m._33;
			mOriginPosition.x = m._41;
			mOriginPosition.y = m._42;
			mOriginPosition.z = m._43;
		}
	}

	void CRay::setDirection(const Vec3F& vec)
	{
		mDirection = vec;
	}

	void CRay::setOrigin(const Vec3F& vec)
	{
		mOriginPosition = vec;
	}

	void CRay::setRange(float fRange)
	{
		mRange = fRange;
	}

	bool CRay::intersectAABB(const CAABBox& aabb)
	{
#if 0
		if ((mOriginPosition.x >= aabb.mBoundBoxPit.mMinPos.x && mOriginPosition.x <= aabb.mBoundBoxPit.mMaxPos.x) &&
			(mOriginPosition.y >= aabb.mBoundBoxPit.mMinPos.y && mOriginPosition.y <= aabb.mBoundBoxPit.mMaxPos.y) &&
			(mOriginPosition.z >= aabb.mBoundBoxPit.mMinPos.z && mOriginPosition.z <= aabb.mBoundBoxPit.mMaxPos.z))
		{
			return true;
		}
		else
		{
			return false;
		}
#else
		return (Math::intersectTriangle(&mOriginPosition.x, &mDirection.x, &aabb.mBoundBoxPit[1].x, &aabb.mBoundBoxPit[5].x, &aabb.mBoundBoxPit[6].x, &mResult.fDist) ||
			Math::intersectTriangle(&mOriginPosition.x, &mDirection.x, &aabb.mBoundBoxPit[1].x, &aabb.mBoundBoxPit[6].x, &aabb.mBoundBoxPit[2].x, &mResult.fDist) ||

			Math::intersectTriangle(&mOriginPosition.x, &mDirection.x, &aabb.mBoundBoxPit[0].x, &aabb.mBoundBoxPit[1].x, &aabb.mBoundBoxPit[5].x, &mResult.fDist) ||
			Math::intersectTriangle(&mOriginPosition.x, &mDirection.x, &aabb.mBoundBoxPit[0].x, &aabb.mBoundBoxPit[5].x, &aabb.mBoundBoxPit[4].x, &mResult.fDist) ||

			Math::intersectTriangle(&mOriginPosition.x, &mDirection.x, &aabb.mBoundBoxPit[0].x, &aabb.mBoundBoxPit[4].x, &aabb.mBoundBoxPit[7].x, &mResult.fDist) ||
			Math::intersectTriangle(&mOriginPosition.x, &mDirection.x, &aabb.mBoundBoxPit[0].x, &aabb.mBoundBoxPit[7].x, &aabb.mBoundBoxPit[3].x, &mResult.fDist) ||

			Math::intersectTriangle(&mOriginPosition.x, &mDirection.x, &aabb.mBoundBoxPit[2].x, &aabb.mBoundBoxPit[6].x, &aabb.mBoundBoxPit[7].x, &mResult.fDist) ||
			Math::intersectTriangle(&mOriginPosition.x, &mDirection.x, &aabb.mBoundBoxPit[2].x, &aabb.mBoundBoxPit[7].x, &aabb.mBoundBoxPit[3].x, &mResult.fDist) ||

			Math::intersectTriangle(&mOriginPosition.x, &mDirection.x, &aabb.mBoundBoxPit[0].x, &aabb.mBoundBoxPit[3].x, &aabb.mBoundBoxPit[2].x, &mResult.fDist) ||
			Math::intersectTriangle(&mOriginPosition.x, &mDirection.x, &aabb.mBoundBoxPit[0].x, &aabb.mBoundBoxPit[2].x, &aabb.mBoundBoxPit[1].x, &mResult.fDist) ||

			Math::intersectTriangle(&mOriginPosition.x, &mDirection.x, &aabb.mBoundBoxPit[4].x, &aabb.mBoundBoxPit[7].x, &aabb.mBoundBoxPit[6].x, &mResult.fDist) ||
			Math::intersectTriangle(&mOriginPosition.x, &mDirection.x, &aabb.mBoundBoxPit[4].x, &aabb.mBoundBoxPit[6].x, &aabb.mBoundBoxPit[5].x, &mResult.fDist));
	
		/*	if (mMinPos.x > aabb.mBoundBoxPit.mMaxPos.x)
			{
				return false;
			}
			if (mMaxPos.x < aabb.mBoundBoxPit.mMinPos.x)
			{
				return false;
			}

			if (mMinPos.y > aabb.mBoundBoxPit.mMaxPos.y)
			{
				return false;
			}
			if (mMaxPos.y < aabb.mBoundBoxPit.mMinPos.y)
			{
				return false;
			}

			if (mMinPos.z > aabb.mBoundBoxPit.mMaxPos.z)
			{
				return false;
			}
			if (mMaxPos.z < aabb.mBoundBoxPit.mMinPos.z)
			{
				return false;
			}
			return false;*/
#endif
	}

	bool CRay::intersect(const CSphere& sphere)
	{
		auto&& vec = mOriginPosition - sphere.getPosition();
		if (vec.length() <= sphere.getRadius())
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	bool CRay::intersectTriangle(const Vec3F* v0, const Vec3F* v1, const Vec3F* v2)
	{
		// Find vectors for two edges sharing vert0
		Vec3F&& edge1 = *v1 - *v0;
		Vec3F&& edge2 = *v2 - *v0;

		// Begin calculating determinant - also used to calculate U parameter
		Vec3F pvec;
		Math::vec3Cross(&pvec, &mDirection, &edge2);

		// If determinant is near zero, ray lies in plane of triangle
		float det = Math::vec3Dot(&edge1, &pvec);

		Vec3F tvec;
		if (det > 0)
		{
			tvec = mOriginPosition - *v0;
		}
		else
		{
			tvec = *v0 - mOriginPosition;
			det = -det;
		}

		if (det < 0.0001f)
		{
			return false;
		}
		// Calculate U parameter and test bounds
		float u = Math::vec3Dot(&tvec, &pvec);
		if (u < 0.0f || u > det)
		{
			return false;
		}

		// Prepare to test V parameter
		Vec3F qvec;
		Math::vec3Cross(&qvec, &tvec, &edge1);
	
		// Calculate V parameter and test bounds
		float v = Math::vec3Dot(&mDirection, &qvec);
		if (v < 0.0f || u + v > det)
		{
			return false;
		}
		// Calculate t, scale parameters, ray intersects triangle
		float t = Math::vec3Dot(&edge2, &qvec);
		float fInvDet = 1.0f / det;
		mResult.fDist = t * fInvDet;
		mResult.u = u * fInvDet;
		mResult.u = v * fInvDet;
		mResult.bPick = true;
		return true;
	}

	bool CRay::intersectActorEntity(CActorEntity* pActorEntity)
	{
		if (nullptr != pActorEntity)
		{
			auto pComponet = pActorEntity->getComponent<CCollision>();
			if (nullptr != pComponet)
			{
				const Matrix44& matView = *mpCameraEntity->getViewMatrixPtr();

				// Get the inverse view matrix
				Matrix44 m;
				Math::matrixInverse(&m, &matView);

				auto vDirection = mDirection;
				auto vOrigin = mOriginPosition;

				float t = FLT_MAX;
				auto& aabb = pActorEntity->getBoundBox();
				if (intersectAABB(aabb))
				{
					for (auto item : pActorEntity->getModels())
					{
						if (!(item.second)->isVisual())
						{
							continue;
						}
						auto& modelAABB = (item.second)->getBoundBox();
						if (intersectAABB(modelAABB))
						{
							auto& meshsInfo = (item.second)->getAllMeshInfo();
							for (auto& child : meshsInfo)
							{
								if (nullptr != child.pMeshInstance)
								{
									auto& meshAABB = child.pMeshInstance->getBoundBox();
									if (intersectAABB(meshAABB))
									{
										const Matrix44& matWorld = child.pMeshInstance->getMT();
										Matrix44 mWorldView = matWorld * matView;
										Matrix44 meshMT;
										Math::matrixInverse(&meshMT, &mWorldView);

										mDirection.x = mPickScreenPos.x * meshMT._11 + mPickScreenPos.y * meshMT._21 + mPickScreenPos.z * meshMT._31;
										mDirection.y = mPickScreenPos.x * meshMT._12 + mPickScreenPos.y * meshMT._22 + mPickScreenPos.z * meshMT._32;
										mDirection.z = mPickScreenPos.x * meshMT._13 + mPickScreenPos.y * meshMT._23 + mPickScreenPos.z * meshMT._33;
										mOriginPosition.x = meshMT._41;
										mOriginPosition.y = meshMT._42;
										mOriginPosition.z = meshMT._43;

										auto pMesh = child.pMeshInstance->getMesh();
										auto indices = pMesh->getIndices();
										for( int i =0; i < static_cast<int>(indices.size()); i += 3 )
										{
											auto& v0 = pMesh->getVertex(i + 0);
											auto& v1 = pMesh->getVertex(i + 1);
											auto& v2 = pMesh->getVertex(i + 2);
											if (intersectTriangle(&v0, &v1, &v2))
											{
												if (mResult.fDist < t)
												{
													t = mResult.fDist;
												}
												mResult.fDist = t;
												mResult.dwFace = i / 3;
											}
										}
										mDirection = vDirection;
										mOriginPosition = vOrigin;
									}
								}
							}
						}
					}
				}
			}
		}
		return mResult.bPick;
	}

	CActorEntity* CRay::intersectActorEntities(CActorEntity* pActorEntity, int nCount)
	{
		float fDistance = FLT_MAX;
		CActorEntity* pPickActor = nullptr;
		for( int i = 0; i < nCount; ++i )
		{
			if (intersectActorEntity(pActorEntity + i))
			{
				if (mResult.fDist < fDistance)
				{
					fDistance = mResult.fDist;
					pPickActor = pActorEntity + i;
				}
			}
		}
		return pPickActor;
	}

}