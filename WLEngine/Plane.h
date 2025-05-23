#pragma once
#include "Common.h"
#include "Vector.h"

namespace WL
{
	struct SPlane
	{
		SPlane();
		explicit SPlane(const float* pf);
		SPlane(float a, float b, float c, float d);
		SPlane(const Vec3F& n, float d);
		bool isFrontFacing(const Vec3F& v) const;
		bool isBackFacing(const Vec3F& v) const;
		bool isInPlane(const Vec3F& v) const;
		// casting
		operator float* ();
		operator const float* () const;
		
		//////////////////////////////////////////////////////////////////////////
		//data
		Vec3F n;
		float d = 0;
	};
}