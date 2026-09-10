#pragma once
#include "Common.h"
#include "Matrix.h"
#include "Vector.h"
#include "Plane.h"
#include "Ray.h"
#include "Quaternion.h"
#include "MathLibSSE.h"

namespace WL
{
	namespace Math
	{
		//! returns if a equals b, taking possible rounding errors into account
		template<typename T>
		INLINE bool equals(T a, T b, T tolerance = (T)ROUNDING_ERROR_f32)
		{
			return (a + tolerance >= b) && (a - tolerance <= b);
		}

		INLINE float getLengthsqr(const Vec3F& v0, const Vec3F& v1)
		{
			return ((v0.x - v1.x) * (v0.x - v1.x) +
				(v0.y - v1.y) * (v0.y - v1.y) +
				(v0.z - v1.z) * (v0.z - v1.z));
		}

		INLINE float getLength(const Vec3F& v0, const Vec3F& v1)
		{
			return sqrt(getLengthsqr(v0, v1));
		}

		INLINE float angleToRadian(float value)
		{
			return value * (float)PI / 180.0f;
		}

		INLINE float radianToAngle(float value)
		{
			return value * 180.0f / (float)PI;
		}

		extern Vec3F lerp(const Vec3F& vFrom, const Vec3F& vTo, float t);

		extern CQuaternion lerp(const CQuaternion& vFrom, const CQuaternion& vTo, float t);


		extern void buildMatrixLookAtLH(InOut SMatrix44* pOut, const Vec3F* Eye, const Vec3F* LookAt, const Vec3F* Up);

		extern void buildMatrixPerspectiveFovLH(InOut SMatrix44* pOut, float fovy, float Aspect, float fNear, float fFar);

		extern void buildOrthographicMatrixLH(InOut SMatrix44* pOut, float fWidth, float fHeight, float fNear, float fFar);

		extern void computeTriNormal(const Vec3F& v1, const Vec3F& v2, const Vec3F& v3, InOut Vec3F& normalVec);

		//--------------------------
		// Vector
		//--------------------------
		extern Vec3F vec3Projective(const Vec3F& vFrom, const Vec3F& vTo);
		
		extern Vec3F vec3NomalizeProjective(const Vec3F& vFrom, const Vec3F& vTo);
				
		extern float vec3Radian(const Vec3F& vFrom, const Vec3F& vTo);

		extern float vec3Angle(const Vec3F& vFrom, const Vec3F& vTo, const Vec3F& vDirNormal);

		extern float vec3Dot(const Vec3F* pV1, const Vec3F* pV2);
		
		extern float vec3DotRadian(const Vec3F* pV1, const Vec3F* pV2);
		
		extern float vec3DotAngle(const Vec3F* pV1, const Vec3F* pV2);

		extern void vec3Cross(InOut Vec3F* pOut, const Vec3F* pV1, const Vec3F* pV2);

		extern void vec3RotationX(InOut Vec3F* pVec3, float radian);

		extern void vec3RotationY(InOut Vec3F* pVec3, float radian);

		extern void vec3RotationZ(InOut Vec3F* pVec3, float radian);

		extern void vec3Transform(InOut Vec3F* pInOut, In const SMatrix44* pMatrix);

		extern void vec4Transform(InOut Vec4F* pInOut, In const SMatrix44* pMatrix);

		//--------------------------
		// Matrix
		//--------------------------
		extern void matrixTranspose(Out SMatrix44* pOut, In const SMatrix44* pIn);

		extern void matrixTranslation(InOut SMatrix44* pOut, float x, float y, float z);

		extern void matrixScale(InOut SMatrix44* pOut, float x, float y, float z);

		extern void matrixRotationX(InOut SMatrix44* pOut, float radian);

		extern void matrixRotationY(InOut SMatrix44* pOut, float radian);

		extern void matrixRotationZ(InOut SMatrix44* pOut, float radian);

		extern void matrixRotationEulerian(InOut SMatrix44* pOut, const Vec3F& radian);

		extern void matrixScaling(InOut SMatrix44* pOut, float x, float y, float z);

		extern void matrixIdentity(InOut SMatrix44* pOut);
		
		extern bool matrixInverse(InOut SMatrix44* pOut, IN const SMatrix44* pMatrix);

		extern void matrixToEulerian_XYZ(In SMatrix44* pIn, Out float &x, Out float &y, Out float&z );

		extern void matrixToEulerian_ZYX(In SMatrix44* pIn, Out float& x, Out float& y, Out float& z );

		extern Vec4F matrixMulVec4(In const SMatrix44& matrix, In const Vec4F& v);

		extern Vec3F matrixMulVec3(In const SMatrix44& matrix, In const Vec3F& v);

		//--------------------------
		// Plane
		//--------------------------
		extern float planeDotCoord(In SPlane& plane, In const Vec3F& v);

		extern void PlaneFromPoints(InOut SPlane& plane, const Vec3F& p0, const Vec3F& p1, const Vec3F& p2 );

		extern void PlaneFromPointNormal(InOut SPlane& plane, const Vec3F& p, const Vec3F& normal);

		extern bool intersectTriangle(const float* orig, const float* dir, const float* v0, const float* v1, const float* v2, float* t, float* u, float* v);

		extern bool intersectTriangle(const float* orig, const float* dir, const float* v0, const float* v1, const float* v2, float* t);
		
		extern bool inSideAABB(const CAABBox& bigAABB, const CAABBox& smallAABB);
	}
}