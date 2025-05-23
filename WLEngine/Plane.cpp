#include "Plane.h"
#include "MathLib.h"
#include "Foundation.h"
namespace WL
{
	SPlane::SPlane()
	{

	}

	SPlane::SPlane(const float* pf)
	{
		n.x = pf[0];
		n.y = pf[1];
		n.z = pf[2];
		d = pf[3];
	}

	SPlane::SPlane(float a, float b, float c, float d)
	{
		n.x = a;
		n.y = b;
		n.z = c;
		this->d = d;
	}

	SPlane::SPlane(const Vec3F& n, float d)
	{
		this->n = n;
		this->d = d;
	}

	bool SPlane::isFrontFacing(const Vec3F& v) const
	{
		const float fd = Math::vec3Dot(&n, &v) + d;
		return F32_GREATER_0(fd);
	}

	bool SPlane::isBackFacing(const Vec3F& v) const
	{
		const float fd = Math::vec3Dot(&n, &v) + d;
		return F32_LOWER_0(fd);
	}

	bool SPlane::isInPlane(const Vec3F& v) const
	{
		const float fd = Math::vec3Dot(&n, &v) + d;
		return Foundation::equals(fd, 0.0f);
	}

	SPlane::operator float* ()
	{
		return &n.x;
	}

	SPlane::operator const float* () const
	{
		return &n.x;
	}
}