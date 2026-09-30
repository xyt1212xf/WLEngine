#include "MeshAttributes.h"
namespace ML
{

	void CMeshAttributes::BakeFromDescription(const FMeshDescription& Desc)
	{
		Positions.Empty();
		Normals.Empty();
		Tangents.Empty();
		UVs.Empty();
		Colors.Empty();
		Indices.Empty();
		Sections.Empty();

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

			Positions.Add(V.Position);
			Normals.Add(VI.Normal);
			Tangents.Add(VI.Tangent);
			UVs.Add(VI.UV0);
			Colors.Add(VI.Color);
		}

		const int32 NumPG = Desc.NumPolygonGroups();
		const int32 NumTri = Desc.NumTriangles();

		for (int32 g = 0; g < NumPG; ++g)
		{
			const FMeshPolygonGroup& PG = Desc.PolygonGroups[g];

			FStaticMeshSection Section;
			Section.MaterialIndex = (PG.MaterialIndex != INDEX_NONE) ? PG.MaterialIndex : 0;
			Section.FirstIndex = (UINT32)Indices.Num();

			for (int32 t = 0; t < NumTri; ++t)
			{
				const FMeshTriangle& Tri = Desc.Triangles[t];
				if (Tri.PolygonGroupID == g)
				{
					Indices.Add(Tri.VertexInstanceID[0]);
					Indices.Add(Tri.VertexInstanceID[1]);
					Indices.Add(Tri.VertexInstanceID[2]);
					++Section.NumTriangles;
				}
			}

			// Skip groups without triangles: they would produce zero-size
			// sections and useless draw calls.
			if (Section.NumTriangles > 0)
			{
				Sections.Add(Section);
			}
		}
	}

	bool CMeshAttributes::IsEmpty() const
	{
		return Positions.IsEmpty() && Indices.IsEmpty();
	}

	bool CMeshAttributes::IsValid() const
	{
		return !IsEmpty()
			&& !Sections.IsEmpty()
			&& (Indices.Num() % 3) == 0;
	}
}