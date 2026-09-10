#include "VoxelTerrainBuild.h"
#include "Vector.h"
namespace WL
{

	CVoxelTerrainBuild::CVoxelTerrainBuild()
	{

	}

	CVoxelTerrainBuild::~CVoxelTerrainBuild()
	{

	}

	void CVoxelTerrainBuild::setChunkRange(const Vec3F& pos, int nSizeX, int nSizeY, int nSizeZ)
	{
		mAllChunkArray.resize(nSizeX);
		for (size_t x = 0; x < mAllChunkArray.size(); ++x)
		{
			mAllChunkArray[x].resize(nSizeY);
			for (size_t y = 0; y < mAllChunkArray[x].size(); ++y)
			{
				mAllChunkArray[x][y].resize(nSizeZ);
			}
		}
		mMin = pos;
		mMax.x = (float)nSizeX * SVoxel::nSize * CVoxelChunk::nSize;
		mMax.y = (float)nSizeY * SVoxel::nSize * CVoxelChunk::nSize;
		mMax.z = (float)nSizeZ * SVoxel::nSize * CVoxelChunk::nSize;
		mSizeX = nSizeX;
		mSizeY = nSizeY;
		mSizeZ = nSizeZ;
	}

	void CVoxelTerrainBuild::setVoxelChunk(int nX, int nY, int nZ, CVoxelChunk* pChunk)
	{
		mAllChunkArray[nX][nY][nZ] = pChunk;
		mAllChunks.push_back(pChunk);
	}

	void CVoxelTerrainBuild::setBrush(CVoxelBrush* pBrush)
	{
		mpBrush = pBrush;
	}

	void CVoxelTerrainBuild::testCube()
	{
		float x = 100.0f;
		float y = 10.0f;
		float z = 100.0f;
		mpBrush->setPosition(Vec3F(x, y, z));
		auto brushAABB = mpBrush->getBoundBox();
		
		for (auto pChunk : mAllChunks)
		{	
			if (pChunk->getAABB().checkBoundBox(brushAABB))
			{
				for (auto pVoxel : pChunk->mVoxels)
				{
					if (brushAABB.checkBoundBox(pVoxel->aabb))
					{
						pVoxel->drawVertices.resize(1);
					}
				}
			}
		}
	}

	void CVoxelTerrainBuild::oceanBiome()
	{

	}
	
	void CVoxelTerrainBuild::planeBiome()
	{
		for (int x = 0; x < mSizeX; ++x)
		{
			for (int z = 0; z < mSizeZ; ++z)
			{
				auto pChunk = mAllChunkArray[x][0][z];
				for (int vx = 0; vx < CVoxelChunk::nSize; ++vx)
				{
					for (int vz = 0; vz < CVoxelChunk::nSize; ++vz)
					{
						pChunk->mVoxelArray[vx][0][vz]->drawVertices.resize(1);
					}
				}
			}
		}
	}

	void CVoxelTerrainBuild::desertBiome()
	{	
		planeBiome();
		int nCount = rand() % 300;
		auto brushPos = mpBrush->getPosition();

		for (int i = 0; i < nCount; ++i)
		{
			int nStep = 2;
			int nHeight = rand() % 100 + 1;

			Vec3F newVec3;
			newVec3.x = rand() % (int)mMax.x + mMin.x;
			newVec3.z = rand() % (int)mMax.z + mMin.z;

			for (int y = 0; y < nHeight; y += nStep)
			{
				newVec3.y = (float)y;
				mpBrush->setPosition(newVec3);
				const auto& brushAABB = mpBrush->getBoundBox();
				for (int x = 0; x < mSizeX; ++x)
				{
					for (int y = 0; y < mSizeY; ++y)
					{
						for (int z = 0; z < mSizeZ; ++z)
						{
							auto pChunk = mAllChunkArray[x][y][z];

							if (pChunk->checkBrushCollide(mpBrush))
							{
								for (auto pVoxel : pChunk->mVoxels)
								{
									if (brushAABB.checkBoundBox(pVoxel->aabb))
									{
										pVoxel->drawVertices.resize(1);
									}
								}
							}
						}
					}
				}
			}
		}
		mpBrush->setPosition(brushPos);
	}

	void CVoxelTerrainBuild::forestBiome()
	{
		planeBiome();
		Vec3F newVec3;
		newVec3.x = 50 + mMin.x;
		newVec3.z = 50 + mMin.z;
		int nHill = rand() % 20 + 10;
		int nStep = 2;
		for (int y = 0; y < nHill; y += nStep)
		{
			newVec3.y = (float)y;
			float l = rand() % 20 + 2.0f;
			buildCircleSolide(newVec3, Vec3F(l, 0, 0));
		}
	}

	void CVoxelTerrainBuild::startBuildTerrain()
	{
		for (auto pChunk : mAllChunks)
		{
			for( auto pVoxel : pChunk->mVoxels )
			{
				pVoxel->drawVertices.clear();
			}
		}

		for (int x = 0; x < mSizeX; ++x)
		{
			for (int z = 0; z < mSizeZ; ++z)
			{
				auto pChunk = mAllChunkArray[x][0][z];

			}
		}
	}

	void CVoxelTerrainBuild::buildTerrain()
	{
		for( auto item : mAllChunks )
		{
			item->buildChunkEntity();
		}
	}

	void CVoxelTerrainBuild::buildCircleSolide(const Vec3F& pos, const Vec3F& radiou)
	{
		Vec3F normal = radiou;
		normal.normalize();
		auto l = radiou.length() / 10;
		float nStep = 1.0f;
		do
		{
			auto r = radiou - normal * 10 * nStep++;
			buildCircle(pos, r);
		} while (--l > 0);
	}

	void CVoxelTerrainBuild::buildCircle(const Vec3F& pos, const Vec3F& radiou)
	{
		auto brushPos = mpBrush->getPosition();
		for (int i = 0; i < 360; ++i)
		{
			auto newpos = radiou;
			Math::vec3RotationY(&newpos, Math::angleToRadian((float)i));
			newpos += pos;
			mpBrush->setPosition(newpos);
			const auto& brushAABB = mpBrush->getBoundBox();

			for (auto pChunk : mAllChunks)
			{
				if (pChunk->checkBrushCollide(mpBrush))
				{
					for (auto pVoxel : pChunk->mVoxels)
					{
						if (brushAABB.checkBoundBox(pVoxel->aabb))
						{
							pVoxel->drawVertices.resize(1);
						}
					}
				}
			}
		}
		mpBrush->setPosition(brushPos);
	}
}