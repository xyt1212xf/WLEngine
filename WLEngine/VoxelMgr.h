#pragma once
#include "voxelBrush.h"
#include "VoxelChunk.h"
#include "VoxelTerrainBuild.h"
#include "WLEngine.h"

namespace WL
{
	enum class VoxelRuleEnum : UINT8
	{
		Infinite = 1,
	};

	//����һ�����㲻ͬ�ķ��飬�о�����ͬ�ķ��鲻���з���������
	struct ChunkInfo
	{
		bool   isLoad:1;
		INT16  voxelType:15;		
		std::list<Vec3F> voxels;
	};
	
	class CVoxelMgr : public CRefcount
	{
		friend class CVoxelTerrainEditor;
	public:
		CVoxelMgr();
		~CVoxelMgr();
		void createVoxelTerrain(int nSizeX, int nSizeY, int nSizeZ);
		void setPositon(const Vec3F& pos);
		const Vec3F& getPosition() const;
		void setBrush(CVoxelBrush* pBrush);
		void buildTerrain(int nType);
		void createChunk(const Vec3F& pos, const std::list<Vec3F>& voxels);
		void setProduceVoxlerule(VoxelRuleEnum type);
		VoxelRuleEnum getProduceVoxlerule() const;

	private:
		VoxelRuleEnum mType = VoxelRuleEnum::Infinite;
		Vec3F	mPosition = { 0,0,0 };
		Vec3F	mMinPosition = { 0,0,0 };
		Vec3F	mMaxPosition = { 0, 0, 0 };
		std::list<CVoxelChunk*>	mpChunks;
		CVoxelTerrainBuild	mTerrainBuild;
	};
}

