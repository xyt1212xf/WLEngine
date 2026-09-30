#pragma once
#include "Vector.h"

namespace ML
{
	struct FMeshVertexInstance
	{
		// 默认值给一个"安全"的状态：法线朝 +Z，tangent 朝 +X，UV 在原点，Color 不透明白。
		INT32 VertexID = INDEX_NONE;
		Vec3F Normal = Vec3F(0.0f, 0.0f, 1.0f);
		Vec4F Tangent = Vec4F(1.0f, 0.0f, 0.0f, 1.0f); // w = handedness
		Vec4F Color = Vec4F(1.0f, 1.0f, 1.0f, 1.0f);
		
		Vec2F UV0 = Vec2F(0,0);
	};
}