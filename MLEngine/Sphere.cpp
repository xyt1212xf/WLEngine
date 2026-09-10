#include "Sphere.h"
namespace WL
{
	CSphere::CSphere(const Vec3F& vec, float fRadius)
	{
		setPosition(vec);
		setRadius(fRadius);
	}

	CSphere::CSphere(const CSphere& vec)
	{
		mOrigin = vec.mOrigin;
		mRadius = vec.mRadius;
	}

	CSphere::CSphere(CSphere&& vec)
	{
		mOrigin = vec.mOrigin;
		mRadius = vec.mRadius;
	}

	void CSphere::setPosition(const Vec3F& vec)
	{
		mOrigin = vec;
	}

	void CSphere::setRadius(float fRadius)
	{
		mRadius = fRadius;
	}

	float CSphere::getRadius()const
	{
		return mRadius;
	}

	const Vec3F& CSphere::getPosition() const
	{
		return mOrigin;
	}

}