#pragma once
#include "CameraEntity.h"
#include "Export.h"
#include "GameApp.h"
#include "Entity.h"
#include "ScriptEntity.h"
#include "Entity.h"
#include "ActorEntity.h"
#include "LightEntity.h"
#include "SkyEntity.h"
#include "TerrainEntity.h"
#include "OceanEntity.h"
#include "tinyxml.h"
#include "jsonHeader.h"
#include "Message.h"
#include "VoxelMgr.h"
namespace WL
{
	class CScene : public CScriptEntity
	{
		friend class CEngine;
	public:
		CScene();
		virtual ~CScene();
		bool initialise(const char* szFile);
		bool addEntity(CEntity* pEntity);
		bool removeEntity(CEntity* pEntity);
		void addLight(CLightEntity* pLight);
		void removeLight(CLightEntity* pLight);
		void setVoxelMgr(CVoxelMgr* pVoxelMgr);
		std::list<CGuiEntity*>& getGuis();
		std::list<CLightEntity*>& getLights();
		std::list<CCameraEntity*>& getCameras();
		std::list<CActorEntity*>& getActors();
		std::list<CActorEntity*>& getShowActors(UINT32 nFlag = 0xFFFFFFFF);
		CCameraEntity* getMainCamera() const;

	private:
		void draw(UINT32 dTime);
		void _update(UINT32 dTime);
		void initCamera(const json::reference& value, SWindowConfig* config);
		void initEntities(const json& value, SWindowConfig* config);
		void initChunks(const json& value, SWindowConfig* config);
		void initLight(const json::reference& value, SWindowConfig* config);
		void initSky(const json::reference& value, SWindowConfig* config);
		void initSkyBox(const json::reference& value, SWindowConfig* config);
		void initBornIn(const json::reference& value, SWindowConfig* config);
		void initTerrain(const json::reference& value, SWindowConfig* config);
		void initGui(const json::reference& value, SWindowConfig* config);
		void initOcean(const json& value, CEntity* pEntity);
		void initActor(const json& value, CEntity* pEntity);
		void checkVoxleChunk();

	private:
		std::string					mSceneName;
		std::string					mSaveName;
		std::list<CEntity*>			mEntities;
		std::list<CActorEntity*>	mActors;
		std::list<CActorEntity*>	mShowActors;
		std::list<CLightEntity*>	mLights;
		std::list<CCameraEntity*>	mCameras;
		std::list<CGuiEntity*>		mGuis;
		std::list<CActorEntity*>	mPickActors;
		std::map<Vec3F, SChunkInfo>	mChunks;
		Vec3F	mBornPos;
		CCameraEntity* mpMainCamera = nullptr;
		CSkyEntity* mpSkyEntity = nullptr;
		CSkyBoxEntity* mpSkyBoxEntity = nullptr;
		COceanEntity* mpOceanEntity = nullptr;
		CTerrainEntity* mpTerrainEntity = nullptr;
		CVoxelMgr* mpVoxelMgr = nullptr;
		DeclareScriptClass(CScene)
	};
}