#pragma once
#include "TSingle.h"
#include "Controller.h"
#include "VoxelMgr.h"

namespace WL
{
	struct  SFace
	{
		Vec3I pos[3];
		Vec3F normal;
		SVertexFormatVoxel vPos[3];
		bool operator == (const SFace& box)
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
		bool operator() (const SFace& left, const SFace& right) const
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

		bool operator < (const SFace& e) const
		{
			return pos[0].x < pos[0].x;
		}
 	};
	struct   SGeometryInfo
	{
		//Data
		SVertexFormatVoxel vertices[CVoxelChunk::nSize][CVoxelChunk::nSize][CVoxelChunk::nSize];
		int indices[CVoxelChunk::nSize][CVoxelChunk::nSize][CVoxelChunk::nSize];
		float vertexNormalCount[CVoxelChunk::nSize][CVoxelChunk::nSize][CVoxelChunk::nSize];
		Vec3F vertexNormal[CVoxelChunk::nSize][CVoxelChunk::nSize][CVoxelChunk::nSize];
		Vec2F vertexUV[CVoxelChunk::nSize][CVoxelChunk::nSize][CVoxelChunk::nSize];
		Vec3F faceNormal[CVoxelChunk::nSize][CVoxelChunk::nSize][CVoxelChunk::nSize];
		std::list<SFace> faces;
		//Function
		SGeometryInfo()
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
							  , public SController
	{
		struct SVetexNormal
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
		virtual bool onEvent(SEvent& e) final;
		void generateVoxelChunk(SEvent& e);
		void _generateMesh();
		void _generateTexture();
		
		void drawNormal(CVoxelChunk* pChunk, SGeometryInfo& geometry);
		void mouseBrushPlane(SEvent& e);
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
		std::map<INT64, SGeometryInfo> mChunkGeometryInfo;
	};


}