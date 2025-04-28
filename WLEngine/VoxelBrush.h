#pragma once
#include "AABBox.h"

namespace WL
{
	struct Voxel;
	class CVoxelChunk;
	class CActorEntity;
	class CVoxelBrush
	{
	public:
		CVoxelBrush();
		virtual ~CVoxelBrush();
		virtual void init();
		void setPosition(const Vec3F& pos);
		const Vec3F& getPosition() const;
		CAABBox getBoundBox()const;
		void setVisible(bool bValue);
		virtual std::pair<bool,Vec3F> getIsoSurfaceVec3(CVoxelChunk* pChunk, Voxel* pVoxel);
		const std::string& getTexture(int nIndex);

	protected:
		CActorEntity* mpBrushEntity = nullptr;
		std::vector<std::string>	mTextureName;
	};


	class CSqhereBrush : public CVoxelBrush
	{
	public:
		CSqhereBrush();
		virtual ~CSqhereBrush();
		virtual void init();
		virtual std::pair<bool, Vec3F> getIsoSurfaceVec3(CVoxelChunk* pChunk, Voxel* pVoxel) final;
		Vec3F mPosition;
	};
}