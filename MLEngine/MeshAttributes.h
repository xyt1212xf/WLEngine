#pragma once
#include "Vector.h"
#include "MeshDescription.h"

namespace ML
{
	struct FStaticMeshSection
	{
		INT32  MaterialIndex = 0;   // index into the owning CStaticMesh's MaterialSlots
		UINT32 FirstIndex = 0;   // start position within the LOD index buffer
		UINT32 NumTriangles = 0;   // triangle count of this chunk
	};

	class MLENGINE_API CMeshAttributes
	{
	public:
		void BakeFromDescription(const FMeshDescription& Desc);

		bool IsEmpty() const;
		bool IsValid() const;

		// Flat vertex streams. All have the same length = number of vertex instances.
		const std::vector<Vec3F>& GetPositions() const { return Positions; }
		const std::vector<Vec3F>& GetNormals()   const { return Normals; }
		const std::vector<Vec4F>& GetTangents()  const { return Tangents; }
		const std::vector<Vec2F>& GetUVs()       const { return UVs; }
		const std::vector<Vec4F>& GetColors()    const { return Colors; }

		// Index buffer content: 3 per triangle, grouped by section.
		const std::vector<UINT32>& GetIndices() const { return Indices; }

		// One section per non-empty PolygonGroup.
		const std::vector<FStaticMeshSection>& GetSections() const { return Sections; }

		UINT32 GetNumVertices() const { return (UINT32)Positions.size(); }
		UINT32 GetNumIndices()  const { return (UINT32)Indices.size(); }

	protected:
		std::vector<Vec3F> Positions;
		std::vector<Vec3F> Normals;
		std::vector<Vec4F> Tangents;   // xyz = tangent direction, w = handedness
		std::vector<Vec2F> UVs;        // single UV set (simplified)
		std::vector<Vec4F> Colors;

		std::vector<UINT32> Indices;                 // 3 per triangle, grouped by section
		std::vector<FStaticMeshSection> Sections;    // one per non-empty PolygonGroup
	};
}
