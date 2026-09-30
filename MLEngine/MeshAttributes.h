#pragma once
#include "Vector.h"
#include "Array.h"
#include "MeshDescription.h"

namespace ML
{
	struct FStaticMeshSection
	{
		INT32  MaterialIndex = 0;   // index into the owning CStaticMesh's MaterialSlots
		UINT32 FirstIndex = 0;   // start position within the LOD index buffer
		UINT32 NumTriangles = 0;   // triangle count of this chunk
	};

	class CMeshAttributes
	{
	public:
		void BakeFromDescription(const FMeshDescription& Desc);

		bool IsEmpty() const;
		bool IsValid() const;

		// Flat vertex streams. All have the same length = number of vertex instances.
		const TArray<Vec3F>& GetPositions() const { return Positions; }
		const TArray<Vec3F>& GetNormals()   const { return Normals; }
		const TArray<Vec4F>& GetTangents()  const { return Tangents; }
		const TArray<Vec2F>& GetUVs()       const { return UVs; }
		const TArray<Vec4F>& GetColors()    const { return Colors; }

		// Index buffer content: 3 per triangle, grouped by section.
		const TArray<UINT32>& GetIndices() const { return Indices; }

		// One section per non-empty PolygonGroup.
		const TArray<FStaticMeshSection>& GetSections() const { return Sections; }

		UINT32 GetNumVertices() const { return (UINT32)Positions.Num(); }
		UINT32 GetNumIndices()  const { return (UINT32)Indices.Num(); }

	protected:
		TArray<Vec3F> Positions;
		TArray<Vec3F> Normals;
		TArray<Vec4F> Tangents;   // xyz = tangent direction, w = handedness
		TArray<Vec2F> UVs;        // single UV set (simplified)
		TArray<Vec4F> Colors;

		TArray<UINT32> Indices;                 // 3 per triangle, grouped by section
		TArray<FStaticMeshSection> Sections;    // one per non-empty PolygonGroup
	};
}