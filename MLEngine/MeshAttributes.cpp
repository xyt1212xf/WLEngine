#include "MeshAttributes.h"
namespace ML
{

	void CMeshAttributes::BakeFromDescription(const FMeshDescription& Desc)
	{
		Positions.clear();
		Normals.clear();
		Tangents.clear();
		UVs.clear();
		Colors.clear();
		Indices.clear();
		Sections.clear();

		// Reject invalid topology: every triangle must reference valid vertex
		// instances and an assigned material slot (see FMeshDescription::Validate).
		if (!Desc.Validate())
		{
			return;
		}

		const int32 NumVI = Desc.NumVertexInstances();
		for (int32 i = 0; i < NumVI; ++i)
		{
			const FMeshVertexInstance& VI = Desc.VertexInstances[i];
			const FMeshVertex& V = Desc.Vertices[VI.VertexID];

			Positions.push_back(V.Position);
			Normals.push_back(VI.Normal);
			Tangents.push_back(VI.Tangent);
			UVs.push_back(VI.UV0);
			Colors.push_back(VI.Color);
		}

		const int32 NumPG = Desc.NumPolygonGroups();
		const int32 NumTri = Desc.NumTriangles();

		for (int32 g = 0; g < NumPG; ++g)
		{
			const FMeshPolygonGroup& PG = Desc.PolygonGroups[g];

			FStaticMeshSection Section;
			Section.MaterialIndex = (PG.MaterialIndex != INDEX_NONE) ? PG.MaterialIndex : 0;
			Section.FirstIndex = (UINT32)Indices.size();

			for (int32 t = 0; t < NumTri; ++t)
			{
				const FMeshTriangle& Tri = Desc.Triangles[t];
				if (Tri.PolygonGroupID == g)
				{
					Indices.push_back(Tri.VertexInstanceID[0]);
					Indices.push_back(Tri.VertexInstanceID[1]);
					Indices.push_back(Tri.VertexInstanceID[2]);
					++Section.NumTriangles;
				}
			}

			// Skip groups without triangles: they would produce zero-size
			// sections and useless draw calls.
			if (Section.NumTriangles > 0)
			{
				Sections.push_back(Section);
			}
		}
	}

	bool CMeshAttributes::IsEmpty() const
	{
		return Positions.empty() && Indices.empty();
	}

	bool CMeshAttributes::IsValid() const
	{
		return !IsEmpty()
			&& !Sections.empty()
			&& (Indices.size() % 3) == 0;
	}
}
