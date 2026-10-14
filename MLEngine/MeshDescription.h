#pragma once
#include "MeshVertex.h"
#include "MeshVertexInstance.h"
#include "MeshTriangle.h"
#include "MeshPolygonGroup.h"
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
		int32 NumVertices()        const { return (int32)Vertices.size(); }
		int32 NumVertexInstances() const { return (int32)VertexInstances.size(); }
		int32 NumTriangles()       const { return (int32)Triangles.size(); }
		int32 NumPolygonGroups()   const { return (int32)PolygonGroups.size(); }

		
		std::vector<FMeshVertex>         Vertices;         // 位置数组（去重后的"逻辑顶点"）
		std::vector<FMeshVertexInstance> VertexInstances;  // 每个角点一份属性（法线/UV/颜色）
		std::vector<FMeshTriangle>       Triangles;        // 三角形列表
		std::vector<FMeshPolygonGroup>   PolygonGroups;    // 材质槽分组
	};
}
