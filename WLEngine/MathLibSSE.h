#pragma once
#include "Common.h"
#include "Matrix.h"
#include "Vector.h"
#include "Plane.h"
#include "Ray.h"
namespace WL
{
	namespace Math
	{

		//inline float reciprocalSSE(const float f)
		//{
		//	/* [2 * rcpps(f) - (f * rcpps(f) * rcpps(f))] */
		//	float rec;
		//	__asm
		//	{
		//		rcpss xmm0, f;               // xmm0 = rcpss(f)
		//		movss xmm1, f;              // xmm1 = f
		//		mulss xmm1, xmm0;            // xmm1 = f * rcpss(f)
		//		mulss xmm1, xmm0;            // xmm2 = f * rcpss(f) * rcpss(f)
		//		addss xmm0, xmm0;            // xmm0 = 2 * rcpss(f)
		//		subss xmm0, xmm1;            // xmm0 = 2 * rcpss(f)
		//									//        - f * rcpss(f) * rcpss(f)
		//		movss rec, xmm0;             // return xmm0
		//	}
		//	return rec;
		//}

		////--------------------------
		//// Vector
		////--------------------------
		//extern float vec3DotSSE(const Vec3F* pV1, const Vec3F* pV2);

		//extern void vec3CrossSSE(InOut Vec3F* pOut, const Vec3F* pV1, const Vec3F* pV2);

		////--------------------------
		//// Matrix
		////--------------------------
		//extern Vec4F matrixMulVec4SSE(In const Matrix44& matrix, In const Vec4F& v);

		//extern Vec3F matrixMulVec3SSE(In const Matrix44& matrix, In const Vec3F& v);

		//extern bool matrixInverseSSE(InOut Matrix44* pOut, IN const Matrix44* pMatrix);
		////--------------------------
		//// Plane
		////--------------------------

	}
}