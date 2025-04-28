#pragma once
#include "Component.h"
#include "AABBox.h"
namespace WL
{
	class CCollision : public CComponent
	{
	public:
		CCollision();
		virtual ~CCollision();
		void setPosition(const Vec3F& pos);
		void setAABBRange(const Vec3F& v0, const Vec3F& v1);
		void setAABB(const CAABBox& aabb);
		const CAABBox& getAABB() const;

	private:
		CAABBox	mAABB;
	};
}