#include "MeshDescription.h"
namespace ML
{
	void FMeshDescription::Empty()
	{
		Triangles.Empty();
		VertexInstances.Empty();
		Vertices.Empty();
		PolygonGroups.Empty();
	}

	bool FMeshDescription::IsEmpty() const
	{
		return Vertices.IsEmpty()
			&& VertexInstances.IsEmpty()
			&& Triangles.IsEmpty()
			&& PolygonGroups.IsEmpty();
	}

	bool FMeshDescription::Validate() const
	{
		const int32 NumVI = NumVertexInstances();
		const int32 NumPG = NumPolygonGroups();

		// PolygonGroups 至少要有一个，否则渲染时没有 Section 可用。
		if (NumPG == 0)
		{
			return false;
		}

		const int32 NumTri = NumTriangles();
		for (int32 i = 0; i < NumTri; ++i)
		{
			const FMeshTriangle& Tri = Triangles[i];

			// 三个 VertexInstanceID 都必须合法
			for (int32 k = 0; k < 3; ++k)
			{
				if (Tri.VertexInstanceID[k] >= (UINT32)NumVI)
				{
					return false;
				}
			}

			// PolygonGroupID 必须合法（INDEX_NONE 也算非法 —— 必须分配材质槽）
			if (Tri.PolygonGroupID < 0 || Tri.PolygonGroupID >= NumPG)
			{
				return false;
			}
		}

		return true;
	}

}