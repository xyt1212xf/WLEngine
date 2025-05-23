#pragma once
#include "Entity.h"
#include "Transform.h"
#include "InputMgr.h"
#include "Vector.h"

namespace WL
{
	class CCameraEntity;
	struct STerrainChunk
	{
		CActorEntity* mActorPtr = nullptr;
		UINT8 mFlag = 0;
	};

	class CTerrainEntity : public CEntity
						 , public CController
						 , public CTranformComponet

	{
	public:
		CTerrainEntity();
		virtual ~CTerrainEntity();

	private:
		virtual void _tick(UINT32 dt) override;
		void updateChunk(UINT32 dt);

	private:
		Vec3F mCameraChunkPos = { 0,0,0 };
		CCameraEntity* mCameraEntityPtr = nullptr;
		float mRangeRadius = 10;

		std::map<Vec3F, STerrainChunk>	mChunkMgr;
		std::list<STerrainChunk>	mShowActorEntities;
	};
}