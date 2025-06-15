#pragma once
#include "Entity.h"
#include "Transform.h"
#include "InputMgr.h"
#include "Vector.h"

namespace WL
{
	class CCameraEntity;
	struct STerrainChunk : public CRefcount
	{
		CActorEntity* mActorPtr = nullptr;
		UINT8 mFlag = 0;
		UINT32 mHideStartTime = 0;
	};

	class CTerrainEntity : public CEntity
						 , public SController
						 , public CTranformComponet

	{
	public:
		CTerrainEntity();
		virtual ~CTerrainEntity();

	private:
		virtual void _tick(UINT32 dt) override;
		void updateChunk(UINT32 dt);
		STerrainChunk* createTerrainChunk(const Vec3F& pos);

	private:
		Vec3F mCameraChunkPos = { 0,0,0 };
		CCameraEntity* mCameraEntityPtr = nullptr;
		float mRangeRadius = 10;

		std::map<Vec3F, STerrainChunk*>	mChunkMgr;
		std::list<STerrainChunk*>	mShowChunks;
		std::list<STerrainChunk*>	mHideChunks;
	};
}