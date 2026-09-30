#include "StaticMeshDescription.h"

namespace ML
{
	CStaticMeshDescription::~CStaticMeshDescription()
	{

	}

	void CStaticMeshDescription::CreateCube(Vec3F Center, Vec3F HalfExtents)
	{
		FMeshDescription& Desc = OwnedMeshDescription;
		Desc.Empty();

		// -------------------------------------------------------------------
		// Vertices: the 8 cube corners.
		// Index layout:
		//   bit0: +x, bit1: +y, bit2: +z
		// -------------------------------------------------------------------
		for (int32 Corner = 0; Corner < 8; ++Corner)
		{
			FMeshVertex V;
			V.Position.x = Center.x + ((Corner & 1) ? +HalfExtents.x : -HalfExtents.x);
			V.Position.y = Center.y + ((Corner & 2) ? +HalfExtents.y : -HalfExtents.y);
			V.Position.z = Center.z + ((Corner & 4) ? +HalfExtents.z : -HalfExtents.z);
			Desc.Vertices.Add(V);
		}

		// -------------------------------------------------------------------
		// One PolygonGroup: the whole cube uses a single material slot.
		// -------------------------------------------------------------------
		FMeshPolygonGroup PG;
		PG.Name = "Cube";
		PG.MaterialIndex = 0;
		const INT32 GroupID = Desc.PolygonGroups.Add(PG);

		// -------------------------------------------------------------------
		// Faces: 6 quads, each defined by 4 corner indices (CCW when viewed
		// from outside), a face normal and 4 UV corners.
		// Per quad: 4 VertexInstances (each pointing back to its corner
		// VertexID, carrying face normal + UV) and 2 Triangles.
		// Winding: CCW front face (default graphics API setting).
		// -------------------------------------------------------------------
		struct SCubeFace
		{
			INT32 Corners[4];      // vertex ids, CCW seen from outside
			Vec3F Normal;
		};

		const SCubeFace Faces[6] =
		{
			// -Z (back):  x-, y-, x+, y+ corners
			{ { 0, 2, 3, 1 }, Vec3F( 0.0f,  0.0f, -1.0f) },
			// +Z (front): x-, y-, x+, y+
			{ { 4, 5, 7, 6 }, Vec3F( 0.0f,  0.0f,  1.0f) },
			// -X (left)
			{ { 0, 4, 6, 2 }, Vec3F(-1.0f,  0.0f,  0.0f) },
			// +X (right)
			{ { 1, 3, 7, 5 }, Vec3F( 1.0f,  0.0f,  0.0f) },
			// -Y (bottom)
			{ { 0, 1, 5, 4 }, Vec3F( 0.0f, -1.0f,  0.0f) },
			// +Y (top)
			{ { 2, 6, 7, 3 }, Vec3F( 0.0f,  1.0f,  0.0f) },
		};

		const Vec2F UVCorners[4] =
		{
			Vec2F(0.0f, 0.0f),
			Vec2F(1.0f, 0.0f),
			Vec2F(1.0f, 1.0f),
			Vec2F(0.0f, 1.0f),
		};

		for (int32 f = 0; f < 6; ++f)
		{
			const SCubeFace& Face = Faces[f];

			// 4 vertex instances for this face. Corner positions are shared
			// through VertexID; normal/UV belong to the instance, which is
			// what makes hard cube edges possible (one vertex, up to three
			// different normals).
			UINT32 VI[4] = { 0, 0, 0, 0 };
			for (int32 k = 0; k < 4; ++k)
			{
				FMeshVertexInstance Instance;
				Instance.VertexID = Face.Corners[k];
				Instance.Normal   = Face.Normal;
				Instance.UV0      = UVCorners[k];
				VI[k] = (UINT32)Desc.VertexInstances.Add(Instance);
			}

			// Split the quad into 2 CCW triangles: (0,1,2) and (0,2,3).
			FMeshTriangle TriA;
			TriA.VertexInstanceID[0] = VI[0];
			TriA.VertexInstanceID[1] = VI[1];
			TriA.VertexInstanceID[2] = VI[2];
			TriA.PolygonGroupID = GroupID;
			Desc.Triangles.Add(TriA);

			FMeshTriangle TriB;
			TriB.VertexInstanceID[0] = VI[0];
			TriB.VertexInstanceID[1] = VI[2];
			TriB.VertexInstanceID[2] = VI[3];
			TriB.PolygonGroupID = GroupID;
			Desc.Triangles.Add(TriB);
		}
	}

}