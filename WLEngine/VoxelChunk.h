#pragma once
#include <bitset>
#include "Vector.h"
#include "AABBox.h"
#include "ActorEntity.h"
#include "voxelBrush.h"
namespace WL
{
	struct Voxel
	{
		//data
		const static int nSize = 1;
		VertexFormatVoxel	baseVertices[24];
		std::vector<VertexFormatVoxel> drawVertices;
		std::vector<INT32> drawIndices;
		CAABBox	aabb;
		Vec3F vec = { 0, 0, 0 };
		int nX = -1;
		int nY = -1;
		int nZ = -1;
		bool bShow = false;

		//Function
		void setVoxelSize(const Vec3F& fMin, const Vec3F& fMax);
	};


	class CVoxelChunk
	{
		friend class CVoxelMgr;
		friend class CVoxelTerrainBuild;

	public:
		CVoxelChunk();
		~CVoxelChunk();
		void buildChunkEntity();
		void generateGeometry();
		void createVoxel(const Vec3F& position);
		void createChunk(const Vec3F& position, const std::list<Vec3F>& voxels);
		void createChunk(const Vec3F& position, int layer, Axis nFlag);
		const Vec3I& getPosition() const;
		const Vec3F& getCenter() const;

		UINT64 getUUID() const;
		bool checkBrushCollide(CVoxelBrush* pBrush);
		const CAABBox& getAABB()const;
		std::vector<Voxel*>& getVoxels();

	private:
		void setUUID(const Vec3I& pos);
		void setPosition(const Vec3I& pos);
		bool isFillVoxel(int x, int y, int z);
		void fillVoxelIndex(int x, int y, int z);
		void unFillVoxelIndex(int x, int y, int z);

	public:
		const static int nSize = 32;
		/*mUUID的位数分布
		x区域(17位)   y区域(15位)  z区域(17位)	  x在chunk的索引(5位)		y在chunk的索引(5位)		z在chunk的索引(5位)
		1...1		 1...1		 1...1		  11111111				11111111				11111111
		*/
		union
		{
			//结构大小64位
			struct
			{
				INT8	mChunkX : 5;		//0-31
				INT8	mChunkY : 5;		//0-31
				INT8	mChunkZ : 5;		//0-31
				INT32	mSectionX : 17;		//0-131071
				INT32	mSectionY : 15;		//0-32767
				INT32	mSectionZ : 17;		//0-131071
			};
			UINT64		mUUID = 0;
		}; 
		std::bitset<32> mMaskVoxel[3]; //记录chunk里面每个位置是否有voxels;
		Vec3I		mPosition = {0,0,0};
		std::vector<VertexFormatVoxel*> mVertices;
		std::vector<VertexFormatVoxel> mDrawVertices;
		std::vector<UINT32> mDrawIndices;
		CAABBox	mAABB;		
		std::vector< std::vector< std::vector<Voxel*>>> mVoxelArray;
		std::vector<Voxel*> mVoxels;
		CActorEntity* mpActorEntity = nullptr;
		std::string mszName;

#ifdef _DEBUG
		CActorEntity* mpNormalEntity = nullptr;
#endif
	};
}