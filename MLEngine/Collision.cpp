#include "Collision.h"
namespace WL
{

	CCollision::CCollision()
	{
		mComponentName = "Collision";
	}

	CCollision::~CCollision()
	{

	}

	void CCollision::setPosition(const Vec3F& pos)
	{
		mAABB.setPosition(pos);
	}

	void CCollision::setAABBRange(const Vec3F& v0, const Vec3F& v1)
	{
		Vec3F minPos, maxPos;
		if (v0.x < v1.x)
		{
			minPos.x = v0.x;
			maxPos.x = v1.x;
		}
		else
		{
			minPos.x = v0.x;
			maxPos.x = v1.x;
		}

		if (v0.y < v1.y)
		{
			minPos.y = v0.y;
			maxPos.y = v1.y;
		}
		else
		{
			minPos.y = v0.y;
			maxPos.y = v1.y;
		}

		if (v0.z < v1.z)
		{
			minPos.z = v0.z;
			maxPos.z = v1.z;
		}
		else
		{
			minPos.z = v0.z;
			maxPos.z = v1.z;
		}

		mAABB.setBoundBoxRange(minPos, maxPos);
	}

	void CCollision::setAABB(const CAABBox& aabb)
	{
		mAABB = aabb;
	}

	const CAABBox& CCollision::getAABB() const
	{
		return mAABB;
	}

}