#include "VoxelMgr.h"
#include "WLEngine.h"
#include "VoxelTerrainEditor.h"
#include "Scene.h"

namespace WL
{
	CVoxelMgr::CVoxelMgr()
	{

	}

	CVoxelMgr::~CVoxelMgr()
	{
		for( auto item : mpChunks)
		{
			WL_DELETE(item,Voxel);
		}
	}


	void CVoxelMgr::createVoxelTerrain(int nSizeX, int nSizeY, int nSizeZ)
	{
		float fSizeX = CVoxelChunk::nSize * SVoxel::nSize;
		float fSizeY = CVoxelChunk::nSize * SVoxel::nSize;
		float fSizeZ = CVoxelChunk::nSize * SVoxel::nSize;

		Vec3F minPos = { FLT_MAX, FLT_MAX, FLT_MAX };
		Vec3F maxPos = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

		auto modifyMinMaxPos = [&](Vec3F& position)
		{
			if (position.x > maxPos.x)
			{
				maxPos.x = position.x;
			}
			if (position.y > maxPos.y)
			{
				maxPos.y = position.y;
			}
			if (position.z > maxPos.z)
			{
				maxPos.z = position.z;
			}

			if (position.x < minPos.x)
			{
				minPos.x = position.x;
			}
			if (position.y < minPos.y)
			{
				minPos.y = position.y;
			}
			if (position.z < minPos.z)
			{
				minPos.z = position.z;
			}
		};

		mTerrainBuild.setChunkRange(mPosition, nSizeX, nSizeY, nSizeZ);
		Vec3F position = mPosition;
		for (int x = 0; x < nSizeX; ++x)
		{
			position.x = mPosition.x + x * fSizeX;
			for (int y = 0; y < nSizeY; ++y)
			{
				position.y = mPosition.y + y * fSizeY;
				for (int z = 0; z < nSizeZ; ++z)
				{
					position.z = mPosition.z + z * fSizeZ;
					CVoxelChunk* pVoxelChunk = WL_NEW(CVoxelChunk, Voxel);
					pVoxelChunk->createVoxel(Vec3F(position.x - x * SVoxel::nSize,
												   position.y - y * SVoxel::nSize,
												   position.z - z * SVoxel::nSize));

					mpChunks.push_back(pVoxelChunk);
					mTerrainBuild.setVoxelChunk(x, y, z, pVoxelChunk);
					modifyMinMaxPos(position);
				}
			}
		}
		{
			maxPos += Vec3F(CVoxelChunk::nSize, CVoxelChunk::nSize, CVoxelChunk::nSize);
			CVoxelTerrainEditor::gVoxelTerrainAABB.setBoundBoxRange(minPos, maxPos);
			CVoxelTerrainEditor::gVoxelTerrainAABB.buildLineMesh();
			auto pActorEntity = GEngine->createEntity<CActorEntity>(EntityType::Actor);
			pActorEntity->addModel(CVoxelTerrainEditor::gVoxelTerrainAABB.mpModelInstance);
			GEngine->getCurrentScenePtr()->addEntity(pActorEntity);
			WL_DECREASE(pActorEntity);
		}

		{
			maxPos.y = minPos.y + 0.0001f;
			CVoxelTerrainEditor::gVoxelGroundAABB.setBoundBoxRange(minPos, maxPos);
			CVoxelTerrainEditor::gVoxelGroundAABB.buildLineMesh();
			auto pActorEntity = GEngine->createEntity<CActorEntity>(EntityType::Actor);
			pActorEntity->addModel(CVoxelTerrainEditor::gVoxelGroundAABB.mpModelInstance);
			GEngine->getCurrentScenePtr()->addEntity(pActorEntity);
			WL_DECREASE(pActorEntity);
		}
	}

	void CVoxelMgr::setPositon(const Vec3F& pos)
	{
		mPosition = pos;
	}


	const Vec3F& CVoxelMgr::getPosition() const
	{
		return mPosition;
	}

	void CVoxelMgr::setBrush(CVoxelBrush* pBrush)
	{
		mTerrainBuild.setBrush(pBrush);
	}


	void CVoxelMgr::buildTerrain(int nType)
	{
		mTerrainBuild.startBuildTerrain();
		switch (nType)
		{
		case -1:
		{
			mTerrainBuild.testCube();
			break;
		}
		case 0:
		{
			mTerrainBuild.oceanBiome();
			break;
		}
		case 1:
		{
			mTerrainBuild.planeBiome();
			break;
		}
		case 2:
		{
			mTerrainBuild.desertBiome();
			break;
		}
		case 3:
		{
			mTerrainBuild.forestBiome();
			break;
		}
		}
		mTerrainBuild.buildTerrain();
	}

	void CVoxelMgr::createChunk(const Vec3F& pos, const std::list<Vec3F>& voxels)
	{
		CVoxelChunk* pVoxelChunk = WL_NEW(CVoxelChunk, Voxel);
		mpChunks.push_back(pVoxelChunk);
		pVoxelChunk->createChunk(pos, voxels);
		pVoxelChunk->buildChunkEntity();
	}

	void CVoxelMgr::setProduceVoxleRule(VoxelRuleEnum type)
	{
		mType = type;
	}

	VoxelRuleEnum CVoxelMgr::getProduceVoxlerule() const
	{
		return mType;
	}

}
