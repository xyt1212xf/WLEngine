#include "VoxelBrush.h"
#include "ActorEntity.h"
#include "WLEngine.h"
#include "Scene.h"
namespace WL
{
	const int EdgeTable[12][2] =
	{
		{0,1},
		{1,2},
		{2,3},
		{0,3},
		{4,5},
		{5,6},
		{6,7},
		{4,7},
		{0,4},
		{1,5},
		{2,6},
		{3,7}
	};
	CVoxelBrush::CVoxelBrush()
	{
	}

	CVoxelBrush::~CVoxelBrush()
	{

	}


	void CVoxelBrush::init()
	{
		Vec3F vPos = { 0, 0, 0 };
		mpBrushEntity = GEngine->createEntity<CActorEntity>(EntityType::Actor);
		mpBrushEntity->addModel("Resource/Model/cubeBrush.md", 0);
		mpBrushEntity->addComponent<CCollision>();
		mpBrushEntity->setPosition(vPos);
		mpBrushEntity->setName("brushEntity");
		auto pModelInstance = mpBrushEntity->getModelInstance();
		auto pMatInstance = pModelInstance->getMaterialInstance();
		pMatInstance->setAmbientColor(Vec4F(1, 0, 0, 1));
		GEngine->getCurrentScenePtr()->addEntity(mpBrushEntity);
	}


	void CVoxelBrush::setPosition(const Vec3F& pos)
	{
		mpBrushEntity->setPosition(pos);
	}


	const Vec3F& CVoxelBrush::getPosition() const
	{
		return mpBrushEntity->getPosition();
	}


	CAABBox CVoxelBrush::getBoundBox() const
	{
		return mpBrushEntity->getBoundBox();
	}


	void CVoxelBrush::setVisible(bool bValue)
	{
		if (nullptr != mpBrushEntity)
		{
			mpBrushEntity->setVisible(bValue);
		}
	}


	std::pair<bool, Vec3F> CVoxelBrush::getIsoSurfaceVec3(CVoxelChunk* pChunk, Voxel* pVoxel)
	{
		auto& AABB = mpBrushEntity->getBoundBox();
		int nFlag[8] = { 1,1,1,1,1,1,1,1 };
		for (int i = 0; i < 8; ++i)
		{
			if (AABB.checkPoint(pVoxel->baseVertices[i].position))
			{
				nFlag[i] = 0;
			}
		}
		std::pair<bool, Vec3F> r;

		auto& isoV = r.second;
		isoV = { 0, 0, 0 };
		float nCount = 0;
		for (int i = 0; i < 12; ++i)
		{
			int p0 = EdgeTable[i][0];
			int p1 = EdgeTable[i][1];
			if (nFlag[p0] != nFlag[p1])
			{
				isoV += ((pVoxel->baseVertices[p0].position + pVoxel->baseVertices[p1].position) * 0.5f);
				++nCount;
			}
		}
		
		if (nCount > 0)
		{
			isoV /= (float)nCount;
			r.first = true;
		}
		else
		{
			isoV.x = -1;
			r.first = false;
		}

		return r;
	}


	const std::string& CVoxelBrush::getTexture(int nIndex)
	{
		if (nIndex < (INT32)mTextureName.size())
		{
			return mTextureName[nIndex];
		}
		else
		{
			static std::string szDefault = "";
			return szDefault;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	CSqhereBrush::CSqhereBrush()
	{

	}

	CSqhereBrush::~CSqhereBrush()
	{

	}

	void CSqhereBrush::init()
	{

	}


	std::pair<bool, Vec3F> CSqhereBrush::getIsoSurfaceVec3(CVoxelChunk* pChunk, Voxel* pVoxel)
	{	
		std::pair<bool, Vec3F> r;
		r.first = false;
		r.second = {0, 0, 0};
		return r;
		//mPosition + v0;
	}

}