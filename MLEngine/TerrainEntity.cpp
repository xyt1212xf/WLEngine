#include "TerrainEntity.h"
#include "CameraEntity.h"
#include "WLEngine.h"
#include "Foundation.h"
namespace WL
{
	CTerrainEntity::CTerrainEntity()
	{
		mType = Terrain;
		mCameraEntityPtr = GEngine->getMainCameraEntity();
		mCameraChunkPos = mCameraEntityPtr->getEye();
		//减掉任何数字，让第一次摄像机和chunkPos的位置不一样。否者不会动态载入地形
		mCameraChunkPos.x -= 10;
		WL_INCREASE(mCameraEntityPtr);
	}

	CTerrainEntity::~CTerrainEntity()
	{
		WL_DECREASE(mCameraEntityPtr);
		mCameraEntityPtr = nullptr;
		for (auto item : mChunkMgr)
		{
			WL_DELETE(item.second, Model);
		}
		mChunkMgr.clear();
		mShowChunks.clear();
		mHideChunks.clear();
	}

	void CTerrainEntity::_tick(UINT32 dt)
	{
		static UINT32 nowTime = ::GetTickCount();
		if (dt - nowTime > 500)
		{
			updateChunk(dt);
		}
	}

	void CTerrainEntity::updateChunk(UINT32 dt)
	{
		float yHeight = 990.f;
		float nSize = 1024;
		const auto& pt = mCameraEntityPtr->getEye();
		Vec3F cameraChunkPos(pt.x / nSize, pt.y / 100, pt.z / nSize);

		if (cameraChunkPos == mCameraChunkPos)
		{
			return;
		}

		mCameraChunkPos = cameraChunkPos;
		for (auto item : mShowChunks)
		{
			item->mFlag = 0;
		}

		INT32 nStartX = cameraChunkPos.x - mRangeRadius;
		INT32 nEndX = cameraChunkPos.x + mRangeRadius;
		INT32 nStartZ = cameraChunkPos.z - mRangeRadius;
		INT32 nEndZ = cameraChunkPos.z / nSize + mRangeRadius;

		INT32 nHeight = cameraChunkPos.y;
		
		std::list<STerrainChunk*> tempShowChunks;

		for (int i = nStartX; i < nEndX; ++i)
		{
			for (int j = nStartZ; j < nEndZ; ++j)
			{
				auto pos = Vec3F(i * nSize, yHeight, j * nSize);
				auto [iter, inserted] = mChunkMgr.try_emplace(pos);
				if (inserted)
				{
					auto* pChunk = createTerrainChunk(pos);
					if (nullptr != pChunk)
					{
						mShowChunks.push_back(pChunk);
						iter->second = pChunk;
					}
					else
					{
						mChunkMgr.erase(iter);
					}
				}
				else
				{
					(iter->second)->mFlag = 1;
					tempShowChunks.push_back(iter->second);					
				}
			}
		}

		UINT32 time = ::GetTickCount();
		auto iter = mShowChunks.begin();
		while (iter != mShowChunks.end())
		{
			if (0 == (*iter)->mFlag)
			{
				(*iter)->mActorPtr->hide();
				(*iter)->mHideStartTime = time;
				mHideChunks.push_back(*iter);
				iter = mShowChunks.erase(iter);
			}
			else
			{
				++iter;
			}
		}
		//交换显示列表
		std::swap(mShowChunks, tempShowChunks);
	}	

	STerrainChunk* CTerrainEntity::createTerrainChunk(const Vec3F& pos)
	{
		if (auto* pChunk = WL_NEW(STerrainChunk, Model))
		{
			if (auto* pActorPtr = WL_NEW(CActorEntity, Entity))
			{
				
				pChunk->mActorPtr = pActorPtr;
			}
			else
			{
				WL_DELETE(pChunk, Model);
			}
			return pChunk;
		}
		return nullptr;
	}
}

