#include "Quaternion.h"
#include "Common.h"
#include "Foundation.h"
namespace WL
{

	CQuaternion CQuaternion::operator+(const CQuaternion& other) const
	{
		return CQuaternion(mx + other.mx, my + other.my, mz + other.mz, mw + other.mw);
	}

	CQuaternion CQuaternion::operator*(float s) const
	{
		return CQuaternion(s * mx, s * my, s * mz, s * mw);
	}

	CQuaternion& CQuaternion::operator*=(float s)
	{
		mx *= s;
		my *= s;
		my *= s;
		mw *= s;
		return *this;
	}

	CQuaternion CQuaternion::operator*(const CQuaternion& other) const
	{
		CQuaternion tmp;

		tmp.mw = (other.mw * mw) - (other.mx * mx) - (other.my * my) - (other.mz * mz);
		tmp.mx = (other.mw * mx) + (other.mx * mw) + (other.my * mz) - (other.mz * my);
		tmp.my = (other.mw * my) + (other.my * mw) + (other.mz * mx) - (other.mx * mz);
		tmp.mz = (other.mw * mz) + (other.mz * mw) + (other.mx * my) - (other.my * mx);
		return tmp;
	}

	void CQuaternion::inverse()
	{
		mx = -mx;
		my = -my;
		mz = -mz;
	}

	void CQuaternion::identity()
	{
		mx = 0.f;
		my = 0.f;
		mz = 0.f;
		mw = 1.f;
	}

	void CQuaternion::toEuler(Vec3F& euler)
	{
		const double sqw = mw * mw;
		const double sqx = mx * mx;
		const double sqy = my * my;
		const double sqz = mz * mz;
		const double test = 2.0 * (my * mw - mx * mz);

		if (Foundation::equals(test, 1.0, 0.000001))
		{
			// heading = rotation about z-axis
			euler.z = (float)(-2.0 * atan2(mx, mw));
			// bank = rotation about x-axis
			euler.x = 0;
			// attitude = rotation about y-axis
			euler.y = (float)(PI / 2.0);
		}
		else if (Foundation::equals(test, -1.0, 0.000001))
		{
			// heading = rotation about z-axis
			euler.z = (float)(2.0 * atan2(mx, mw));
			// bank = rotation about x-axis
			euler.x = 0;
			// attitude = rotation about y-axis
			euler.y = (float)(PI / -2.0);
		}
		else
		{
			// heading = rotation about z-axis
			euler.z = (float)atan2(2.0 * (mx * my + mz * mw), (sqx - sqy - sqz + sqw));
			// bank = rotation about x-axis
			euler.x = (float)atan2(2.0 * (my * mz + mx * mw), (-sqx - sqy + sqz + sqw));
			// attitude = rotation about y-axis
			euler.y = (float)asin(Foundation::clamp(test, -1.0, 1.0));
		}
	}

	CQuaternion& CQuaternion::set(float x, float y, float z, float w)
	{
		mx = x;
		my = y;
		mz = z;
		mw = w;
		return *this;
	}

	CQuaternion& CQuaternion::set(float x, float y, float z)
	{
		double angle;

		angle = x * 0.5;
		const double sr = sin(angle);
		const double cr = cos(angle);

		angle = y * 0.5;
		const double sp = sin(angle);
		const double cp = cos(angle);

		angle = z * 0.5;
		const double sy = sin(angle);
		const double cy = cos(angle);

		const double cpcy = cp * cy;
		const double spcy = sp * cy;
		const double cpsy = cp * sy;
		const double spsy = sp * sy;

		mx = (float)(sr * cpcy - cr * spsy);
		my = (float)(cr * spcy + sr * cpsy);
		mz = (float)(cr * cpsy - sr * spcy);
		mw = (float)(cr * cpcy + sr * spsy);

		return normalize();
	}

	WL::CQuaternion& CQuaternion::normalize()
	{
		const float n = mx * mx + my * my + mz * mz + mw * mw;

		if (n == 1)
		{
			return *this;
		}
		return (*this *= (1.f / sqrtf(n)));
	}

}

