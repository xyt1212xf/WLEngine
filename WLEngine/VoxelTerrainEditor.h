#pragma once
#include "TSingle.h"
#include "Controller.h"
#include "VoxelMgr.h"

namespace WL
{
	struct  Face
	{
		Vec3I pos[3];
		Vec3F normal;
		VertexFormatVoxel vPos[3];
		bool operator == (const Face& box)
		{
			if (pos[0] != box.pos[0])
			{
				return false;
			}
			if (pos[1] != box.pos[1])
			{
				return false;
			}
			if (pos[2] != box.pos[2])
			{
				return false;
			}
			return true;
		}
		bool operator() (const Face& left, const Face& right) const
		{
			if (left.pos[0] != right.pos[0])
			{
				return left.pos[0].x < right.pos[0].x;
			}
			if (left.pos[0] != right.pos[0])
			{
				return left.pos[0].x < right.pos[0].x;
			}
			if (left.pos[0].x != right.pos[0].x)
			{
				return left.pos[0].x < right.pos[0].x;
			}
			return false;
		}

		bool operator < (const Face& e) const
		{
			return pos[0].x < pos[0].x;
		}
 	};
	struct   GeometryInfo
	{
		//Data
		VertexFormatVoxel vertices[CVoxelChunk::nSize][CVoxelChunk::nSize][CVoxelChunk::nSize];
		int indices[CVoxelChunk::nSize][CVoxelChunk::nSize][CVoxelChunk::nSize];
		float vertexNormalCount[CVoxelChunk::nSize][CVoxelChunk::nSize][CVoxelChunk::nSize];
		Vec3F vertexNormal[CVoxelChunk::nSize][CVoxelChunk::nSize][CVoxelChunk::nSize];
		Vec2F vertexUV[CVoxelChunk::nSize][CVoxelChunk::nSize][CVoxelChunk::nSize];
		Vec3F faceNormal[CVoxelChunk::nSize][CVoxelChunk::nSize][CVoxelChunk::nSize];
		std::list<Face> faces;
		//Function
		GeometryInfo()
		{
			for( int x = 0; x < CVoxelChunk::nSize; ++x)
			{
				for (int y = 0; y < CVoxelChunk::nSize; ++y)
				{
					for (int z = 0; z < CVoxelChunk::nSize; ++z)
					{
						indices[x][y][z] = -1;
					}
				}
			}
		}
	};

	class CVoxelBrush;
	class CVoxelTerrainEditor : public TSingle<CVoxelTerrainEditor>
							  , public CController
	{
		struct vetexNormal
		{
			Vec4I	vPos;
			Vec3F	normals;
			float	nCount = 0;
		};

	public:
		CVoxelTerrainEditor();
		~CVoxelTerrainEditor();
	
	public:
		static CAABBox gVoxelGroundAABB;
		static CAABBox gVoxelTerrainAABB;

	private:
		virtual bool onEvent(event& e) final;
		void generateVoxelChunk(event& e);
		void _generateMesh();
		void _generateTexture();
		
		void drawNormal(CVoxelChunk* pChunk, GeometryInfo& geometry);
		void mouseBrushPlane(event& e);
		void modifyBrushPlaneDir();

	private:
		bool mbDrawTexture = false;
		int mDrawTextureIndex = 1;
		int mMouseFlag = 0;
		int mMouseX = 0;
		int mMouseY = 0;
		CVoxelMgr* mpVoxelMgr = nullptr;
		CVoxelBrush* mpBrush = nullptr;
		CActorEntity* mpBrushPlane = nullptr;
		std::vector<Vec3F>	mGroundPlaneVertice;
		std::map<INT64, GeometryInfo> mChunkGeometryInfo;
	};


}