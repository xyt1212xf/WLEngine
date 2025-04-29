#include "WLEngine.h"
#include "Scene.h"
#include "GameController.h"
#include "SceneController.h"
#include "CameraController.h"
#include "DefMemory.h"
#include "BehaviorTree.h"
#include "GameplayHeader.h"
#include "GameplayTagsManager.h"

#ifdef TERRAIN_EDIT
#include "VoxelTerrainEditor.h"
#endif

namespace WL
{
	CEngine::CEngine()
	{
//		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
		srand((UINT32)time(nullptr));
		GEngine = this;
  	}

	CEngine::~CEngine()
	{
		GEngine = nullptr;
//		_CrtDumpMemoryLeaks();
	}

	void CEngine::update(UINT32 dTime)
	{
		if (mpScene)
		{
			m_SceneMutex.lock();
			mpScene->_update(dTime);
			m_SceneMutex.unlock();
		}
		OPTICK_EVENT();
		CGuiEngine::_update(dTime);
		CScriptEngine::_update(dTime);
		CSoundEngine::_update(dTime);
		CLightEngine::_update(dTime);
		CGraphicEngine::_update(dTime);
	}

	bool CEngine::initialize()
	{
		CMemoryMgr::initialize();

		mbRunning = true;
		mpThreadPools = WL_NEW(CThreadPool, Thread);
		bool bValue = false;
#ifdef _WINDOWS
		bValue = CWinPlatform::initialize();
#endif
		commitCommand(commandType::StopDraw);
		bValue &= CResourceMgr::initialize() &&
				  CGuiEngine::initialize() &&
				  CScriptEngine::initialize() &&
				  CSoundEngine::initialize() &&
				  CEntityMgr::initialize() &&
				  CLightEngine::initialize() &&
				  CGraphicEngine::initialize() && 
				  CInputMgr::initialize() &&
				  CTimerEngine::initialize() &&
				  CFontEngine::initialize();

		
		commitCommand(commandType::RecoverDraw);
		registerController(WL_NEW(CSceneController, Controller)(mpScene), 1);
		registerController(WL_NEW(CGameController, Controller), 2);
		registerController(WL_NEW(CCameraController, Controller)(getMainCameraEntity()), 3);
#ifdef TERRAIN_EDIT
		//auto pInstance = CVoxelTerrainEditor::createInstance();
		//if (nullptr != pInstance)
		//{
		registerController(WL_NEW(CVoxelTerrainEditor, Controller), -1);
		//}
#else
		mpVoxelMgr = WL_NEW(CVoxelMgr, Manager)();
#endif
		CGameplayTagsManager::initializeManager();
		CBehaviorTreeMgr::regBTreeToScrpit();
		doFile("script/GameApp.lua", "start");
		return bValue;
	}

	void CEngine::unInitialize()
	{
		CGameplayTagsManager::unInitializeManager();
		CBehaviorTreeMgr::unRegBTreeToScrpit();
		WL_DECREASE(mpVoxelMgr);
		mbRunning = false;
#ifdef _WINDOWS
		CWinPlatform::unInitialize();
#endif
		CGraphicEngine::unInitialize();
		CGuiEngine::unInitialize();
		CScriptEngine::unInitialize();
		CSoundEngine::unInitialize();
		CLightEngine::unInitialize();
		CResourceMgr::unInitialize();
		CEntityMgr::unInitialize();
		CInputMgr::unInitialize();
		CTimerEngine::unInitialize();
		CFontEngine::unInitialize();
		WL_DELETE(mpScene, Scene);
		WL_DELETE(mpThreadPools, Thread);
		CMemoryMgr::unInitialize();
	}

	bool CEngine::isRun()
	{
		return mbRunning;
	}

	void CEngine::exitGame()
	{
#ifdef  _WINDOWS
		SendMessage(mhMainWnd, WM_DESTROY, 0, 0);
#endif
	}

	CScene* CEngine::createScene()
	{
		WL_DELETE(mpNextScene, Scene);
		mpNextScene = WL_NEW(CScene, Scene);
		return mpNextScene;
	}

	CScene* CEngine::createScene(const std::string& szFile)
	{
		CScene* pScene = createScene();
		std::string sceneFile = szFile;
		if (!pScene->initialise(sceneFile.c_str()))
		{
			WL_DELETE(pScene, Scene);
		}
		return pScene;
	}

	CGameplayTagsManager* CEngine::getGameplayTagsMgr()
	{
		return CGameplayTagsManager::getSinglePtr();
	}


	void CEngine::gotoScene(CScene* pScene)
	{
		mpNextScene = pScene;
	}

	CScene* CEngine::getCurrentScenePtr() const
	{
		return mpScene;
	}

	CScene* CEngine::getNextScenePtr() const
	{
		return mpNextScene;
	}

	CCameraEntity* CEngine::getMainCameraEntity()
	{
		if (nullptr != mpScene)
		{
			return mpScene->getMainCamera();
		}
		else
		{
			return nullptr;
		}
	}

	void CEngine::renderWorld()
	{
		static UINT32 dTime = GetTickCount();
		UINT32 offTime = GetTickCount(); 
		procesCommandBuf();
		if (nullptr != mpNextScene)
		{
			m_SceneMutex.lock();
			WL_DELETE(mpScene, Scene);
			mpScene = mpNextScene;
			m_SceneMutex.unlock();
			mpNextScene = nullptr;
		}
		if (nullptr != mpScene)
		{
			mpScene->draw(offTime - dTime);
		}
		dTime = offTime;
	}

	void CEngine::setVoxelMgr(CVoxelMgr* pMgr)
	{
		if (nullptr != pMgr)
		{
			WL_INCREASE(pMgr);
			WL_DECREASE(mpVoxelMgr);
			mpVoxelMgr = pMgr;
		}
	}

	CVoxelMgr* CEngine::getVoxelMgr() const
	{
		assert(mpVoxelMgr);
		return mpVoxelMgr;
	}

	void CEngine::executeCmd(const std::string& cmd, void* pParams)
	{
		if( "Fps" == cmd)
		{
			auto pScene = getCurrentScenePtr();
			if (nullptr != pScene)
			{
				bool bShow = *static_cast<bool*>(pParams);
				if (bShow)
				{
					auto& pGuis = pScene->getGuis();
					for( auto& gui : pGuis )
					{
						if( gui->getName() == "FPS")
						{
							gui->setVisible(bShow);
							return;
						}
					}
					auto fFPSWnd = WL_NEW(CGuiEntity, Entity);
					fFPSWnd->setName("FPS");
					fFPSWnd->initialise((R"(resource\Gui\FPSWnd.gui)"));
					pScene->addEntity(fFPSWnd);
					fFPSWnd->setVisible(true);
					fFPSWnd->setText(1, "FPS:0");
					WL_DECREASE(fFPSWnd);
				}
			}
		}
	}

}



