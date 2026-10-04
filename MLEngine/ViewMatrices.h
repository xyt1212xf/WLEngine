#pragma once
#include "Matrix.h"
#include "Vector.h"

namespace ML
{
	struct FViewMatrices
	{
		FMatrix44 ViewMatrix;      // 世界 → 相机空间
		FMatrix44 ProjMatrix;      // 相机空间 → 裁剪空间
		FMatrix44 ViewProjMatrix;  // 世界 → 裁剪空间（预乘）

		Vec3F ViewOrigin;       // 相机位置（世界空间）

		// 构造时自动计算 ViewProj
		FViewMatrices(const FMatrix44& InView, const FMatrix44& InProj)
			: ViewMatrix(InView)
			, ProjMatrix(InProj)
			, ViewProjMatrix(InProj* InView)  // 注意：DX 是 Proj * View
		{
		}
	};
}