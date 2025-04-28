#pragma once
#include "voxelBrush.h"
#include "VoxelChunk.h"
namespace WL
{
	const static int edges[12][2] =
	{
		{ 0, 1 },
		{ 1, 2 },
		{ 2, 3 },
		{ 3, 0 },
		{ 4, 5 },
		{ 5, 6 },
		{ 6, 7 },
		{ 7, 4 },
		{ 0, 4 },
		{ 1, 5 },
		{ 2, 6 },
		{ 3, 7 }
	};

	enum BIOME_TYPE
	{
		BIOME_OCEAN = 0,
		BIOME_PLAINS, //1
		BIOME_DESERT,//2
		BIOME_FOREST,//3
		BIOME_EXTREMEHILLS,//4

		BIOME_SWAMPLAND,//5
		BIOME_TAIGA,//6
		BIOME_JUNGLE,//7
		BIOME_ICE_PLAINS,//8
		BIOME_FROZEN_OCEAN,//9

		BIOME_FROZEN_RIVER,//10
		BIOME_REDSOIL,//11
		BIOME_REDSOIL_SHORE,//12
		BIOME_DESERT_HILLS,//13
		BIOME_FOREST_HILLS,//14

		BIOME_TAIGA_HILLS,//15
		BIOME_ICE_MOUNTAINS,//16
		BIOME_JUNGLE_HILLS,//17
		BIOME_RIVER,//18
		BIOME_BEACH,//19
	};

	class CVoxelTerrainBuild
	{
	public:
		CVoxelTerrainBuild();
		~CVoxelTerrainBuild();
		void setChunkRange(const Vec3F& pos, int nSizeX, int nSizeY, int nSizeZ);
		void setVoxelChunk(int nX, int nY, int nZ, CVoxelChunk* pChunk);
		void setBrush(CVoxelBrush* pBrush);
		
		void testCube();

		void oceanBiome();
		void planeBiome();
		void desertBiome();
		void forestBiome();

		void startBuildTerrain();
		void buildTerrain();

	private:
		void buildCircleSolide(const Vec3F& pos, const Vec3F& fRadius);
		void buildCircle(const Vec3F& pos, const Vec3F& fRadius);

	private:
		CVoxelBrush* mpBrush = nullptr;
		std::vector< std::vector<std::vector<CVoxelChunk*>>> mAllChunkArray;
		std::vector<CVoxelChunk*> mAllChunks;
		int mSizeX = 0;
		int mSizeY = 0;
		int mSizeZ = 0;
		Vec3F mMin;
		Vec3F mMax;
	};
}