#pragma once
#include "Common.h"

namespace ML
{
	struct FMeshTriangle
	{
		UINT32 VertexInstanceID[3] = { 0, 0, 0 };

		// 该三角形归属的多边形组（材质槽）下标。
		// INDEX_NONE（-1）表示未分配；UE 内部用 #define INDEX_NONE (-1)。
		INT32  PolygonGroupID = INDEX_NONE;

		// 工具：是否已分配多边形组
		bool IsValid() const { return PolygonGroupID != INDEX_NONE; }
	};
}