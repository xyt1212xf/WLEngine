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
			Desc.Vertices.push_back(V);
		}

		// -------------------------------------------------------------------
		// One PolygonGroup: the whole cube uses a single material slot.
		// -------------------------------------------------------------------
		FMeshPolygonGroup PG;
		PG.Name = "Cube";
		PG.MaterialIndex = 0;
		Desc.PolygonGroups.push_back(PG);
		const INT32 GroupID = (INT32)Desc.PolygonGroups.size() - 1;

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

				// Vertex color: encode the corner octant into RGB (bit0 = +X,
				// bit1 = +Y, bit2 = +Z) so each face gets a 4-corner color
				// gradient and COLOR interpolation becomes visible.
				// Without this the instance keeps its default (1,1,1,1) and the
				// whole cube renders flat white.
				const INT32 Corner = Face.Corners[k];
				Instance.Color = Vec4F(
					(Corner & 1) ? 1.0f : 0.0f,   // R: +X side
					(Corner & 2) ? 1.0f : 0.0f,   // G: +Y side
					(Corner & 4) ? 1.0f : 0.0f,   // B: +Z side
					1.0f);

				VI[k] = (UINT32)Desc.VertexInstances.size();
				Desc.VertexInstances.push_back(Instance);
			}

			// Split the quad into 2 CCW triangles: (0,1,2) and (0,2,3).
			FMeshTriangle TriA;
			TriA.VertexInstanceID[0] = VI[0];
			TriA.VertexInstanceID[1] = VI[1];
			TriA.VertexInstanceID[2] = VI[2];
			TriA.PolygonGroupID = GroupID;
			Desc.Triangles.push_back(TriA);

			FMeshTriangle TriB;
			TriB.VertexInstanceID[0] = VI[0];
			TriB.VertexInstanceID[1] = VI[2];
			TriB.VertexInstanceID[2] = VI[3];
			TriB.PolygonGroupID = GroupID;
			Desc.Triangles.push_back(TriB);
		}
	}

	void CStaticMeshDescription::CreateSphere(Vec3F Center, float Radius, int32 Slices, int32 Stacks)
	{
		FMeshDescription& Desc = OwnedMeshDescription;
		Desc.Empty();

		// 至少 3 段, 否则极点扇面会退化
		if (Slices < 3)
		{
			Slices = 3;
		}
		if (Stacks < 3)
		{
			Stacks = 3;
		}
		// 整球用一个材质槽
		FMeshPolygonGroup PG;
		PG.Name = "Sphere";
		PG.MaterialIndex = 0;
		Desc.PolygonGroups.push_back(PG);
		const INT32 GroupID = (INT32)Desc.PolygonGroups.size() - 1;

		// 加一个球面点(顶点 + 对应实例), 返回实例 id。
		// 法线用球面法线(LocalPos 归一化), 球面上法线随位置平滑过渡。
		auto AddPoint = [&](const Vec3F& LocalPos, const Vec2F& UV) -> UINT32
		{
			FMeshVertex Vtx;
			Vtx.Position = Center + LocalPos;
			Desc.Vertices.push_back(Vtx);
			const UINT32 Vid = (UINT32)Desc.Vertices.size() - 1;

			Vec3F N = LocalPos;
			const float Len = sqrtf(N.x * N.x + N.y * N.y + N.z * N.z);
			if (Len > 1e-6f) { N.x /= Len; N.y /= Len; N.z /= Len; }
			else N = Vec3F(0.0f, 1.0f, 0.0f);

			FMeshVertexInstance Inst;
			Inst.VertexID = Vid;
			Inst.Normal = N;
			Inst.UV0 = UV;
			Inst.Color = Vec4F(1.0f, 1.0f, 1.0f, 1.0f);
			Desc.VertexInstances.push_back(Inst);
			return (UINT32)Desc.VertexInstances.size() - 1;
		};

		// 顶极点(法线 +Y), 单顶点
		const UINT32 TopPole = AddPoint(Vec3F(0.0f, Radius, 0.0f), Vec2F(0.5f, 0.0f));

		// 中间环: i = 1 .. Stacks-1, 每环 Slices+1 列(含接缝列)
		std::vector<std::vector<UINT32>> Rings;
		Rings.resize(Stacks);
		for (int32 i = 1; i < Stacks; ++i)
		{
			const float V = (float)i / (float)Stacks;
			const float Phi = V * PI;
			const float SinPhi = sinf(Phi);
			const float CosPhi = cosf(Phi);
			Rings[i].resize(Slices + 1);
			for (int32 j = 0; j <= Slices; ++j)
			{
				const float U = (float)j / (float)Slices;
				const float Theta = U * 2.0f * PI;
				Vec3F Local;
				Local.x = Radius * SinPhi * cosf(Theta);
				Local.y = Radius * CosPhi;
				Local.z = Radius * SinPhi * sinf(Theta);
				Rings[i][j] = AddPoint(Local, Vec2F(U, V));
			}
		}

		// 底极点(法线 -Y), 单顶点
		const UINT32 BottomPole = AddPoint(Vec3F(0.0f, -Radius, 0.0f), Vec2F(0.5f, 1.0f));

		// 顶扇面: (TopPole, Ring1[j+1], Ring1[j])
		for (int32 j = 0; j < Slices; ++j)
		{
			FMeshTriangle T;
			T.VertexInstanceID[0] = TopPole;
			T.VertexInstanceID[1] = Rings[1][j + 1];
			T.VertexInstanceID[2] = Rings[1][j];
			T.PolygonGroupID = GroupID;
			Desc.Triangles.push_back(T);
		}

		// 中间环之间的四边形: T1=(A,C,D) T2=(A,B,C), 外法线 CCW
		for (int32 i = 1; i < Stacks - 1; ++i)
		{
			for (int32 j = 0; j < Slices; ++j)
			{
				const UINT32 A = Rings[i][j];
				const UINT32 B = Rings[i][j + 1];
				const UINT32 C = Rings[i + 1][j + 1];
				const UINT32 D = Rings[i + 1][j];

				FMeshTriangle T1;
				T1.VertexInstanceID[0] = A;
				T1.VertexInstanceID[1] = C;
				T1.VertexInstanceID[2] = D;
				T1.PolygonGroupID = GroupID;
				Desc.Triangles.push_back(T1);

				FMeshTriangle T2;
				T2.VertexInstanceID[0] = A;
				T2.VertexInstanceID[1] = B;
				T2.VertexInstanceID[2] = C;
				T2.PolygonGroupID = GroupID;
				Desc.Triangles.push_back(T2);
			}
		}

		// 底扇面: (BottomPole, Ring[Stacks-1][j], Ring[Stacks-1][j+1])
		for (int32 j = 0; j < Slices; ++j)
		{
			FMeshTriangle T;
			T.VertexInstanceID[0] = BottomPole;
			T.VertexInstanceID[1] = Rings[Stacks - 1][j];
			T.VertexInstanceID[2] = Rings[Stacks - 1][j + 1];
			T.PolygonGroupID = GroupID;
			Desc.Triangles.push_back(T);
		}
	}

}
