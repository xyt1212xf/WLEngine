#pragma once
#include "MeshVertex.h"
#include "MeshVertexInstance.h"
#include "MeshTriangle.h"
#include "MeshPolygonGroup.h"
#include "Array.h"
namespace ML
{
	struct FMeshDescription
	{
		FMeshDescription() = default;
		~FMeshDescription() = default;
		void Empty();
		bool IsEmpty() const;
		bool Validate() const;

		// 数量查询（与 UE 保持一致）。
		// FDefaultAllocator 的 SizeType 是 INT32，Num() 直接返回 INT32，无需转换。
		int32 NumVertices()        const { return Vertices.Num(); }
		int32 NumVertexInstances() const { return VertexInstances.Num(); }
		int32 NumTriangles()       const { return Triangles.Num(); }
		int32 NumPolygonGroups()   const { return PolygonGroups.Num(); }

		
		TArray<FMeshVertex>         Vertices;         // 位置数组（去重后的"逻辑顶点"）
		TArray<FMeshVertexInstance> VertexInstances;  // 每个角点一份属性（法线/UV/颜色）
		TArray<FMeshTriangle>       Triangles;        // 三角形列表
		TArray<FMeshPolygonGroup>   PolygonGroups;    // 材质槽分组
	};
}