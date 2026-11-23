#pragma once
#include "Matrix.h"
#include "Vector.h"
namespace ML
{
	struct FConvexVolume
	{
	public:
		static FConvexVolume FromViewProj(const FMatrix44& M)   // UE: FConvexVolume::FromViewProjectionMatrix
		{
			FConvexVolume V;
			const Vec4F C0(M.m[0][0], M.m[1][0], M.m[2][0], M.m[3][0]);
			const Vec4F C1(M.m[0][1], M.m[1][1], M.m[2][1], M.m[3][1]);
			const Vec4F C2(M.m[0][2], M.m[1][2], M.m[2][2], M.m[3][2]);
			const Vec4F C3(M.m[0][3], M.m[1][3], M.m[2][3], M.m[3][3]);
			V.Planes[0] = NormalizePlane(C3 + C0);   // 左
			V.Planes[1] = NormalizePlane(C3 - C0);   // 右
			V.Planes[2] = NormalizePlane(C3 + C1);   // 下
			V.Planes[3] = NormalizePlane(C3 - C1);   // 上
			V.Planes[4] = NormalizePlane(C2);        // 近（LH：z ∈ [0, w]）
			V.Planes[5] = NormalizePlane(C3 - C2);   // 远
			return V;
		}
		bool IntersectSphere(const Vec3F& Center, float Radius) const
		{
			for (const Vec4F& P : Planes)
			{
				if (P.x * Center.x + P.y * Center.y + P.z * Center.z + P.w < -Radius)
				{
					return false;
				}
			}
			return true;
		}

	private:
		static Vec4F NormalizePlane(const Vec4F& P)
		{
			const float Len = sqrtf(P.x * P.x + P.y * P.y + P.z * P.z);
			if (Len > 1e-8f)
			{
				return Vec4F(P.x / Len, P.y / Len, P.z / Len, P.w / Len);
			}
			return P;
		}
	public:
		// 每个平面 (a,b,c,d)：a*x + b*y + c*z + d >= 0 为内侧
		Vec4F Planes[6];
	};

}
