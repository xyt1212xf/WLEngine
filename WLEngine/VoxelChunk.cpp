#include <sstream>
#include <algorithm>
#include "Common.h"
#include "VoxelChunk.h"
#include "WLEngine.h"
#include "Foundation.h"
#include "Scene.h"
namespace WL
{
	CVoxelChunk::CVoxelChunk()
	{

	}

	CVoxelChunk::~CVoxelChunk()
	{
		if (mVoxelArray.size() > 0)
		{
			for (int x = 0; x < nSize; ++x)
			{
				for (int y = 0; y < nSize; ++y)
				{
					for (int z = 0; z < nSize; ++z)
					{
						WL_DELETE(mVoxelArray[x][y][z], Voxel);
					}
				}
			}
		}
		mVoxels.clear();
	}

	void CVoxelChunk::buildChunkEntity()
	{
		mDrawVertices.clear();
		mDrawIndices.clear();
		for (auto item : mVoxels)
		{
			auto pVoxel = item;
			if (pVoxel->drawVertices.size() > 0)
			{
				int nStartIndex = static_cast<int>(mDrawVertices.size());
				mDrawIndices.push_back(nStartIndex + 0);
				mDrawIndices.push_back(nStartIndex + 3);
				mDrawIndices.push_back(nStartIndex + 2);

				mDrawIndices.push_back(nStartIndex + 0);
				mDrawIndices.push_back(nStartIndex + 2);
				mDrawIndices.push_back(nStartIndex + 1);

				mDrawIndices.push_back(nStartIndex + 4);
				mDrawIndices.push_back(nStartIndex + 7);
				mDrawIndices.push_back(nStartIndex + 6);

				mDrawIndices.push_back(nStartIndex + 4);
				mDrawIndices.push_back(nStartIndex + 6);
				mDrawIndices.push_back(nStartIndex + 5);


				mDrawIndices.push_back(nStartIndex + 8);
				mDrawIndices.push_back(nStartIndex + 11);
				mDrawIndices.push_back(nStartIndex + 10);

				mDrawIndices.push_back(nStartIndex + 8);
				mDrawIndices.push_back(nStartIndex + 10);
				mDrawIndices.push_back(nStartIndex + 9);

				mDrawIndices.push_back(nStartIndex + 12);
				mDrawIndices.push_back(nStartIndex + 15);
				mDrawIndices.push_back(nStartIndex + 14);

				mDrawIndices.push_back(nStartIndex + 12);
				mDrawIndices.push_back(nStartIndex + 14);
				mDrawIndices.push_back(nStartIndex + 13);

				mDrawIndices.push_back(nStartIndex + 16);
				mDrawIndices.push_back(nStartIndex + 19);
				mDrawIndices.push_back(nStartIndex + 18);

				mDrawIndices.push_back(nStartIndex + 16);
				mDrawIndices.push_back(nStartIndex + 18);
				mDrawIndices.push_back(nStartIndex + 17);

				mDrawIndices.push_back(nStartIndex + 20);
				mDrawIndices.push_back(nStartIndex + 23);
				mDrawIndices.push_back(nStartIndex + 22);

				mDrawIndices.push_back(nStartIndex + 20);
				mDrawIndices.push_back(nStartIndex + 22);
				mDrawIndices.push_back(nStartIndex + 21);

				mDrawVertices.insert(mDrawVertices.end(), &pVoxel->baseVertices[0], &pVoxel->baseVertices[24]);
			}
		}
		generateGeometry();
	}

	
	void CVoxelChunk::generateGeometry()
	{
#if 0
		auto pModelInstance = Foundation::generateFanGeometry(Vec3F(4, 0, 0), Vec3F(1, 5, 1), Vec3F(5, 3, 0), "resource/Material/newVoxel.ma", "resource/Texture/grass_top.png");
		auto pActorEntity = GEngine->createEntity<CActorEntity>(Actor);
		pActorEntity->addModel(pModelInstance);
		GEngine->getCurrentScenePtr()->addEntity(pActorEntity);
		WL_DECREASE(pModelInstance);
		return;
#else
		if (mDrawVertices.size() > 0)
		{
			auto pActorEntity = GEngine->createEntity<CActorEntity>(Actor);
			auto pModelInstance = Foundation::generateModel((void*)mDrawVertices.data(),
				mDrawVertices.size(),
				sizeof(VertexFormatVoxel),
				(void*)mDrawIndices.data(),
				mDrawIndices.size(),
				sizeof(UINT32),
				"resource/Material/newVoxel.ma",
				"resource/Texture/Voxel/wood.png");
			if (nullptr != pActorEntity && nullptr != pModelInstance)
			{
				pActorEntity->setName("VoxelChunkMesh");
				pActorEntity->addModel(pModelInstance);
				GEngine->getCurrentScenePtr()->addEntity(pActorEntity);
				mpActorEntity = pActorEntity;
			}
			WL_DECREASE(pActorEntity);
			WL_DECREASE(pModelInstance);
		}
#endif
	}

	void CVoxelChunk::createVoxel(const Vec3F& position)
	{
		mAABB.setBoundBoxRange(position, Vec3F(position.x + nSize * (float)Voxel::nSize,
										       position.y + nSize * (float)Voxel::nSize,
											   position.z + nSize * (float)Voxel::nSize) );
		setPosition(Vec3I((int)position.x, (int)position.y, (int)position.z));
		Vec3F fMin;
		Vec3F fMax;
		mVoxelArray.resize(nSize);
		for (int x = 0; x < nSize; ++x)
		{
			mVoxelArray[x].resize(nSize);
			for (int y = 0; y < nSize; ++y)
			{
				mVoxelArray[x][y].resize(nSize);
				for (int z = 0; z < nSize; ++z)
				{
					Voxel* pVoxel = WL_NEW(Voxel, Voxel);

					pVoxel->nX = (int)x;
					pVoxel->nY = (int)y;
					pVoxel->nZ = (int)z;

					fMin.x = mPosition.x + x * (float)Voxel::nSize;
					fMin.y = mPosition.y + y * (float)Voxel::nSize;
					fMin.z = mPosition.z + z * (float)Voxel::nSize;

					fMax.x = fMin.x + Voxel::nSize;
					fMax.y = fMin.y + Voxel::nSize;
					fMax.z = fMin.z + Voxel::nSize;

					pVoxel->setVoxelSize(fMin, fMax);
					
					mVoxelArray[x][y][z] = pVoxel;

					mVoxels.push_back(pVoxel);
				}
			}
		}
	}


	void CVoxelChunk::createChunk(const Vec3F& position, const std::list<Vec3F>& voxels)
	{
		mAABB.setBoundBoxRange(position, Vec3F(	position.x + CVoxelChunk::nSize * (float)Voxel::nSize,
												position.y + CVoxelChunk::nSize * (float)Voxel::nSize,
												position.z + CVoxelChunk::nSize * (float)Voxel::nSize));
		setPosition(Vec3I((int)position.x, (int)position.y, (int)position.z));
		Vec3F fMin;
		Vec3F fMax;
		
		for( auto item : voxels )
		{
			Voxel* pVoxel = WL_NEW(Voxel, Voxel);
			pVoxel->drawVertices.resize(1);
			pVoxel->nX = (int)item.x;
			pVoxel->nY = (int)item.y;
			pVoxel->nZ = (int)item.z;
		
			fMin.x = mPosition.x + item.x * (float)Voxel::nSize;
			fMin.y = mPosition.y + item.y * (float)Voxel::nSize;
			fMin.z = mPosition.z + item.z * (float)Voxel::nSize;

			fMax.x = fMin.x + Voxel::nSize;
			fMax.y = fMin.y + Voxel::nSize;
			fMax.z = fMin.z + Voxel::nSize;

			pVoxel->setVoxelSize(fMin, fMax);
			mVoxels.push_back(pVoxel);
		}
	}

	void CVoxelChunk::createChunk(const Vec3F& position, int layer, Axis nFlag)
	{
		assert(layer < CVoxelChunk::nSize);
		mAABB.setBoundBoxRange(position, Vec3F(position.x + CVoxelChunk::nSize * (float)Voxel::nSize,
			position.y + CVoxelChunk::nSize * (float)Voxel::nSize,
			position.z + CVoxelChunk::nSize * (float)Voxel::nSize));
		setPosition(Vec3I((int)position.x, (int)position.y, (int)position.z));

		auto creatVoxle = [&](int x, int y, int z)
			{
				Vec3F fMin;
				Vec3F fMax;
				Voxel* pVoxel = WL_NEW(Voxel, Voxel);
				pVoxel->drawVertices.resize(1);
				pVoxel->nX = x;
				pVoxel->nY = y;
				pVoxel->nZ = z;

				fMin.x = mPosition.x + x * (float)Voxel::nSize;
				fMin.y = mPosition.y + y * (float)Voxel::nSize;
				fMin.z = mPosition.z + z * (float)Voxel::nSize;

				fMax.x = fMin.x + Voxel::nSize;
				fMax.y = fMin.y + Voxel::nSize;
				fMax.z = fMin.z + Voxel::nSize;

				pVoxel->setVoxelSize(fMin, fMax);
				mVoxels.push_back(pVoxel);
			};
		if(Axis::X == nFlag)
		{
			for( int y = 0; y < CVoxelChunk::nSize; ++y )
			{
				for( int z = 0; z < CVoxelChunk::nSize; ++z )
				{
					creatVoxle(layer, y, z);
				}
			}
		}
		else if( Axis::Y == nFlag )
		{
			for (int x = 0; x < CVoxelChunk::nSize; ++x)
			{
				for (int z = 0; z < CVoxelChunk::nSize; ++z)
				{
					creatVoxle(x, layer, z);
				}
			}
		}
		else if (Axis::Z == nFlag)
		{
			for (int x = 0; x < CVoxelChunk::nSize; ++x)
			{
				for (int y = 0; y < CVoxelChunk::nSize; ++y)
				{
					creatVoxle(x, y, layer);
				}
			}
		}
	}

	const Vec3I& CVoxelChunk::getPosition() const
	{
		return mPosition;
	}


	const Vec3F& CVoxelChunk::getCenter() const
	{
		return mAABB.getCenter();
	}

	UINT64 CVoxelChunk::getUUID() const
	{
		return mUUID;
	}

	bool CVoxelChunk::checkBrushCollide(CVoxelBrush* pBrush)
	{
		const auto& AABB = pBrush->getBoundBox();
		return AABB.checkBoundBox(mAABB);
	}

	std::vector<Voxel*>& CVoxelChunk::getVoxels()
	{
		return mVoxels;
	}

	void CVoxelChunk::setUUID(const Vec3I& pos)
	{
		int nChunkSize = nSize * Voxel::nSize;
		mSectionX = pos.x / nChunkSize;
		mSectionY = pos.y / nChunkSize;
		mSectionZ = pos.z / nChunkSize;

		mChunkX = pos.x % nChunkSize;
		mChunkY = pos.y % nChunkSize;
		mChunkZ = pos.z % nChunkSize;

		std::stringstream sstream;
		sstream << "VoxelChunk" << mUUID;
		mszName = sstream.str().c_str();
	}

	void CVoxelChunk::setPosition(const Vec3I& pos)
	{
#ifdef _DEBUG
		assert(mUUID == 0);
#endif
		mPosition = pos;
		setUUID(pos);
	}

	bool CVoxelChunk::isFillVoxel(int x, int y, int z)
	{
		if (mMaskVoxel[0].test(x) &&
			mMaskVoxel[1].test(y) &&
			mMaskVoxel[2].test(z) )
		{
			return true;
		}
		else
		{
			return false;
		}

	}

	void CVoxelChunk::fillVoxelIndex(int x, int y, int z)
	{
		mMaskVoxel[0].set(x);
		mMaskVoxel[1].set(y);
		mMaskVoxel[2].set(z);
	}

	void CVoxelChunk::unFillVoxelIndex(int x, int y, int z)
	{
		mMaskVoxel[0].reset(x);
		mMaskVoxel[1].reset(y);
		mMaskVoxel[2].reset(z);
	}

	const CAABBox& CVoxelChunk::getAABB() const
	{
		return mAABB;
	}
	//前 右 后 左 上 下    0(top, left)  1(top, right) 2(down, right) 3(down, left)
	void Voxel::setVoxelSize(const Vec3F& fMin, const Vec3F& fMax)
	{
		aabb.setBoundBoxRange(fMin, fMax);
		//前	
		baseVertices[0].position.x = fMin.x;
		baseVertices[0].position.y = fMax.y;
		baseVertices[0].position.z = fMin.z;
		baseVertices[0].uv.x = 0;
		baseVertices[0].uv.y = 0;
		baseVertices[0].normal.x = 0;
		baseVertices[0].normal.y = 1;
		baseVertices[0].normal.z = 0;

		baseVertices[1].position.x = fMax.x;
		baseVertices[1].position.y = fMax.y;
		baseVertices[1].position.z = fMin.z;
		baseVertices[1].uv.x = 1;
		baseVertices[1].uv.y = 0;
		baseVertices[1].normal.x = 0;
		baseVertices[1].normal.y = 1;
		baseVertices[1].normal.z = 0;

		baseVertices[2].position.x = fMax.x;
		baseVertices[2].position.y = fMin.y;
		baseVertices[2].position.z = fMin.z;
		baseVertices[2].uv.x = 1;
		baseVertices[2].uv.y = 1;
		baseVertices[2].normal.x = 0;
		baseVertices[2].normal.y = 1;
		baseVertices[2].normal.z = 0;

		baseVertices[3].position.x = fMin.x;
		baseVertices[3].position.y = fMin.y;
		baseVertices[3].position.z = fMin.z;
		baseVertices[3].uv.x = 0;
		baseVertices[3].uv.y = 1;
		baseVertices[3].normal.x = 0;
		baseVertices[3].normal.y = 1;
		baseVertices[3].normal.z = 0;

		//右
		baseVertices[4].position.x = fMax.x;
		baseVertices[4].position.y = fMax.y;
		baseVertices[4].position.z = fMin.z;
		baseVertices[4].uv.x = 0;
		baseVertices[4].uv.y = 0;
		baseVertices[4].normal.x = 0;
		baseVertices[4].normal.y = 1;
		baseVertices[4].normal.z = 0;

		baseVertices[5].position.x = fMax.x;
		baseVertices[5].position.y = fMax.y;
		baseVertices[5].position.z = fMax.z;
		baseVertices[5].uv.x = 1;
		baseVertices[5].uv.y = 0;
		baseVertices[5].normal.x = 0;
		baseVertices[5].normal.y = 1;
		baseVertices[5].normal.z = 0;

		baseVertices[6].position.x = fMax.x;
		baseVertices[6].position.y = fMin.y;
		baseVertices[6].position.z = fMax.z;
		baseVertices[6].uv.x = 1;
		baseVertices[6].uv.y = 1;
		baseVertices[6].normal.x = 0;
		baseVertices[6].normal.y = 1;
		baseVertices[6].normal.z = 0;

		baseVertices[7].position.x = fMax.x;
		baseVertices[7].position.y = fMin.y;
		baseVertices[7].position.z = fMin.z;
		baseVertices[7].uv.x = 0;
		baseVertices[7].uv.y = 1;
		baseVertices[7].normal.x = 0;
		baseVertices[7].normal.y = 1;
		baseVertices[7].normal.z = 0;


		//后
		baseVertices[8].position.x = fMax.x;
		baseVertices[8].position.y = fMax.y;
		baseVertices[8].position.z = fMax.z;
		baseVertices[8].uv.x = 0;
		baseVertices[8].uv.y = 0;
		baseVertices[8].normal.x = 0;
		baseVertices[8].normal.y = 1;
		baseVertices[8].normal.z = 0;

		baseVertices[9].position.x = fMax.x;
		baseVertices[9].position.y = fMin.y;
		baseVertices[9].position.z = fMax.z;
		baseVertices[9].uv.x = 1;
		baseVertices[9].uv.y = 0;
		baseVertices[9].normal.x = 0;
		baseVertices[9].normal.y = 1;
		baseVertices[9].normal.z = 0;

		baseVertices[10].position.x = fMin.x;
		baseVertices[10].position.y = fMin.y;
		baseVertices[10].position.z = fMax.z;
		baseVertices[10].uv.x = 1;
		baseVertices[10].uv.y = 1;
		baseVertices[10].normal.x = 0;
		baseVertices[10].normal.y = 1;
		baseVertices[10].normal.z = 0;

		baseVertices[11].position.x = fMin.x;
		baseVertices[11].position.y = fMax.y;
		baseVertices[11].position.z = fMax.z;
		baseVertices[11].uv.x = 0;
		baseVertices[11].uv.y = 1;
		baseVertices[11].normal.x = 0;
		baseVertices[11].normal.y = 1;
		baseVertices[11].normal.z = 0;

		//左 
		baseVertices[12].position.x = fMin.x;
		baseVertices[12].position.y = fMax.y;
		baseVertices[12].position.z = fMax.z;
		baseVertices[12].uv.x = 0;
		baseVertices[12].uv.y = 0;
		baseVertices[12].normal.x = 0;
		baseVertices[12].normal.y = 1;
		baseVertices[12].normal.z = 0;

		baseVertices[13].position.x = fMin.x;
		baseVertices[13].position.y = fMax.y;
		baseVertices[13].position.z = fMin.z;
		baseVertices[13].uv.x = 1;
		baseVertices[13].uv.y = 0;
		baseVertices[13].normal.x = 0;
		baseVertices[13].normal.y = 1;
		baseVertices[13].normal.z = 0;

		baseVertices[14].position.x = fMin.x;
		baseVertices[14].position.y = fMin.y;
		baseVertices[14].position.z = fMin.z;
		baseVertices[14].uv.x = 1;
		baseVertices[14].uv.y = 1;
		baseVertices[14].normal.x = 0;
		baseVertices[14].normal.y = 1;
		baseVertices[14].normal.z = 0;

		baseVertices[15].position.x = fMin.x;
		baseVertices[15].position.y = fMin.y;
		baseVertices[15].position.z = fMax.z;
		baseVertices[15].uv.x = 0;
		baseVertices[15].uv.y = 1;
		baseVertices[15].normal.x = 0;
		baseVertices[15].normal.y = 1;
		baseVertices[15].normal.z = 0;


		//上 
		baseVertices[16].position.x = fMin.x;
		baseVertices[16].position.y = fMax.y;
		baseVertices[16].position.z = fMax.z;
		baseVertices[16].uv.x = 0;
		baseVertices[16].uv.y = 0;
		baseVertices[16].normal.x = 0;
		baseVertices[16].normal.y = 1;
		baseVertices[16].normal.z = 0;

		baseVertices[17].position.x = fMax.x;
		baseVertices[17].position.y = fMax.y;
		baseVertices[17].position.z = fMax.z;
		baseVertices[17].uv.x = 1;
		baseVertices[17].uv.y = 0;
		baseVertices[17].normal.x = 0;
		baseVertices[17].normal.y = 1;
		baseVertices[17].normal.z = 0;

		baseVertices[18].position.x = fMax.x;
		baseVertices[18].position.y = fMax.y;
		baseVertices[18].position.z = fMin.z;
		baseVertices[18].uv.x = 1;
		baseVertices[18].uv.y = 1;
		baseVertices[18].normal.x = 0;
		baseVertices[18].normal.y = 1;
		baseVertices[18].normal.z = 0;

		baseVertices[19].position.x = fMin.x;
		baseVertices[19].position.y = fMax.y;
		baseVertices[19].position.z = fMin.z;
		baseVertices[19].uv.x = 0;
		baseVertices[19].uv.y = 1;
		baseVertices[19].normal.x = 0;
		baseVertices[19].normal.y = 1;
		baseVertices[19].normal.z = 0;

		//下 
		baseVertices[20].position.x = fMin.x;
		baseVertices[20].position.y = fMin.y;
		baseVertices[20].position.z = fMax.z;
		baseVertices[20].uv.x = 0;
		baseVertices[20].uv.y = 0;
		baseVertices[20].normal.x = 0;
		baseVertices[20].normal.y = 1;
		baseVertices[20].normal.z = 0;

		baseVertices[21].position.x = fMax.x;
		baseVertices[21].position.y = fMin.y;
		baseVertices[21].position.z = fMax.z;
		baseVertices[21].uv.x = 1;
		baseVertices[21].uv.y = 0;
		baseVertices[21].normal.x = 0;
		baseVertices[21].normal.y = 1;
		baseVertices[21].normal.z = 0;

		baseVertices[22].position.x = fMax.x;
		baseVertices[22].position.y = fMin.y;
		baseVertices[22].position.z = fMin.z;
		baseVertices[22].uv.x = 1;
		baseVertices[22].uv.y = 1;
		baseVertices[22].normal.x = 0;
		baseVertices[22].normal.y = 1;
		baseVertices[22].normal.z = 0;

		baseVertices[23].position.x = fMin.x;
		baseVertices[23].position.y = fMin.y;
		baseVertices[23].position.z = fMin.z;
		baseVertices[23].uv.x = 0;
		baseVertices[23].uv.y = 1;
		baseVertices[23].normal.x = 0;
		baseVertices[23].normal.y = 1;
		baseVertices[23].normal.z = 0;


		vec = (fMin + fMax ) * 0.5f;
	}

}