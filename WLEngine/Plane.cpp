#include "Plane.h"
#include "MathLib.h"
#include "Foundation.h"
namespace WL
{
	Plane::Plane()
	{

	}

	Plane::Plane(const float* pf)
	{
		n.x = pf[0];
		n.y = pf[1];
		n.z = pf[2];
		d = pf[3];
	}

	Plane::Plane(float a, float b, float c, float d)
	{
		n.x = a;
		n.y = b;
		n.z = c;
		this->d = d;
	}

	Plane::Plane(const Vec3F& n, float d)
	{
		this->n = n;
		this->d = d;
	}

	bool Plane::isFrontFacing(const Vec3F& v) const
	{
		const float fd = Math::vec3Dot(&n, &v) + d;
		return F32_GREATER_0(fd);
	}

	bool Plane::isBackFacing(const Vec3F& v) const
	{
		const float fd = Math::vec3Dot(&n, &v) + d;
		return F32_LOWER_0(fd);
	}

	bool Plane::isInPlane(const Vec3F& v) const
	{
		const float fd = Math::vec3Dot(&n, &v) + d;
		return Foundation::equals(fd, 0.0f);
	}

	Plane::operator float* ()
	{
		return &n.x;
	}

	Plane::operator const float* () const
	{
		return &n.x;
	}
}