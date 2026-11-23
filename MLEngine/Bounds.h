#pragma once
#include <cmath>
#include <algorithm>
#include "Matrix.h"
#include "Vector.h"
namespace ML
{
	struct FBoxSphereBounds
	{
		Vec3F Origin = Vec3F(0, 0, 0);        // 包围球心（世界空间）
		Vec3F BoxExtent = Vec3F(0, 0, 0);     // 轴对齐盒半长（UE: BoxExtent）
		float SphereRadius = 0.0f;            // UE: SphereRadius

		static FBoxSphereBounds TransformBy(const FBoxSphereBounds& InBounds, const FMatrix44& M)
		{
			// 行向量约定：p * M。球心直接变换（取 w 行）；
			// UE: FBoxSphereBounds::TransformBy 用"球心 + 半径 * 各轴缩放"近似。
			FBoxSphereBounds OutBounds;
			OutBounds.Origin = Vec3F(
				InBounds.Origin.x * M.m[0][0] + InBounds.Origin.y * M.m[1][0] + InBounds.Origin.z * M.m[2][0] + M.m[3][0],
				InBounds.Origin.x * M.m[0][1] + InBounds.Origin.y * M.m[1][1] + InBounds.Origin.z * M.m[2][1] + M.m[3][1],
				InBounds.Origin.x * M.m[0][2] + InBounds.Origin.y * M.m[1][2] + InBounds.Origin.z * M.m[2][2] + M.m[3][2]);
			const float sx = std::sqrt(M.m[0][0] * M.m[0][0] + M.m[0][1] * M.m[0][1] + M.m[0][2] * M.m[0][2]);
			const float sy = std::sqrt(M.m[1][0] * M.m[1][0] + M.m[1][1] * M.m[1][1] + M.m[1][2] * M.m[1][2]);
			const float sz = std::sqrt(M.m[2][0] * M.m[2][0] + M.m[2][1] * M.m[2][1] + M.m[2][2] * M.m[2][2]);
			OutBounds.BoxExtent = Vec3F(InBounds.BoxExtent.x * sx, InBounds.BoxExtent.y * sy, InBounds.BoxExtent.z * sz);
			OutBounds.SphereRadius = InBounds.SphereRadius * (std::max)((std::max)(sx, sy), sz);
			return OutBounds;
		}
	};
}
