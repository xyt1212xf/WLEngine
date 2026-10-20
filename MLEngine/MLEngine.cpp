#include "MLEngine.h"
#include "GraphicPlug.h"
#include "Mgr.h"
#include "ShaderMgr.h"
#include "MaterialMgr.h"
#include "TimerClock.h"
#include "World.h"
#include "ScriptPlug.h"

//#include "GameController.h"
//#include "SceneController.h"
//#include "CameraController.h"
//#include "DefMemory.h"
//#include "BehaviorTree.h"
//#include "GameplayHeader.h"
//#include "GameplayTagsManager.h"
//#include "UnrealMemory.h"
//#include "GCObjectMgr.h"

//#ifdef TERRAIN_EDIT
//#include "VoxelTerrainEditor.h"
//#endif

namespace ML
{
	CWorld* GetWorld()
	{
		if (GEngine)
		{
			return GEngine->GetWorld();
		}
		else
		{
			return nullptr;
		}
	}

	CEngine::CEngine()
	{
//		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
		srand((UINT32)time(nullptr));
		GEngine = this;
  	}

	CEngine::~CEngine()
	{
		for (CPlug* plug : mPlugs)
		{
			SafeDelete(plug);
		}
		GEngine = nullptr;
//		_CrtDumpMemoryLeaks();
	}

	bool CEngine::Initialise(const SWindowConfig& config)
	{
		mbRunning = true;
		mpThreadPools = new CThreadPool();

		// 1) Create all CMgr subsystems. Order matters: ShaderMgr compiles all
		//    GlobalShaders first, then MaterialMgr (which depends on compiled shaders).
		CShaderMgr* pShaderMgr   = new CShaderMgr();
		CMaterialMgr* pMaterialMgr = new CMaterialMgr();
		mMgrs.push_back(pShaderMgr);
		mMgrs.push_back(pMaterialMgr);
		for (CMgr* Mgr : mMgrs)
		{
			if (!Mgr->Init())
			{
				OutputDebugStringA(("Engine subsystem init failed: " + std::string(Mgr->GetName()) + "\n").c_str());
				SafeDelete(pShaderMgr);
				SafeDelete(pMaterialMgr);
				mMgrs.clear();
				return false;
			}
		}

		CScriptPlug* scriptPlug = new CScriptPlug();
		if (scriptPlug->Initialise())
		{
			mPlugs.push_back(scriptPlug);
		}
		else
		{
			SafeDelete(scriptPlug);
			return false;
		}
		// 2) Create and start the graphics plug (which will register DefaultMaterial etc.).
		//    Only push into mPlugs once Initialise() succeeded - otherwise mPlugs would
		//    keep a dangling pointer and ~CEngine would double-delete it.
		CGraphicPlug* graphicPlug = new CGraphicPlug();
		if (graphicPlug->Initialise())
		{
			mPlugs.push_back(graphicPlug);
			graphicPlug->InitialiseDefaultMaterial();
			mpWorld = new CWorld();
			return true;
		}

		SafeDelete(graphicPlug);
		return false;
	}

	bool CEngine::UnInitialise()
	{
		mbRunning = false;
		SafeDelete(mpThreadPools);
		SafeDelete(mpWorld);
		// Tear down plugs first (graphic plug still owns device + DefaultMaterial references).
		for (CPlug* plug : mPlugs)
		{
			SafeDelete(plug);
		}
		mPlugs.clear();
		// Shutdown subsystems in reverse order.
		for (int32 i = (int32)mMgrs.size() - 1; i >= 0; --i)
		{
			if (mMgrs[i])
			{
				mMgrs[i]->Shutdown();
				SafeDelete(mMgrs[i]);
			}
		}
		mMgrs.clear();
		return true;
	}


	bool CEngine::ProcessMsg(SEvent& e)
	{
		return false;
	}

	void CEngine::Start()
	{
		for (CPlug* plug : mPlugs)
		{
			plug->Start();
		}
	}

	void CEngine::Run(float deltaSeconds)
	{
		static CTimerClock timeColor;
		timeColor.begin();
		for (const CPlug* plug : mPlugs)
		{
			plug->Process(deltaSeconds);
		}

		if (mpWorld)
		{
			mpWorld->TickLoad();   // 推进异步加载

			if (mpWorld->IsReady())
			{
				mpWorld->Tick(deltaSeconds);     // 正常游戏逻辑
			}
		}
	//	double castTime = timeColor.getTimerMilliSec();
	//	std::cout<<castTime<<"\n";
		timeColor.reset();
	}

	bool CEngine::IsRun()
	{
		return mbRunning;
	}

	bool CEngine::LoadScene(const std::string& name)
	{
		return true;
	}

	//void CEngine::LoadSceneAsync(FOnSceneLoaded func, const std::string& name)
	//{
	//	if (nullptr == mpWorld)
	//	{
	//		mpWorld = new CWorld("GameLevel");

	//	}
	//	if (mpWorld)
	//	{
	//		GEngine->threadDetach(func, name);
	//		//CThreadPool::enqueueDetach(func, bResult, name);
	//	}
	//}

	CWinPlatform& CEngine::GetPlatform()
	{
		return mPlatform;
	}



//
//	void CEngine::update(UINT32 dTime)
//	{
//		//if (mpScene)
//		//{
//		//	m_SceneMutex.lock();
//		//	mpScene->_update(dTime);
//		//	m_SceneMutex.unlock();
//		//}
//		//OPTICK_EVENT();
//		//CGuiEngine::_update(dTime);
//		//CScriptEngine::_update(dTime);
//		//CSoundEngine::_update(dTime);
//		//CLightEngine::_update(dTime);
//		//CGraphicEngine::_update(dTime);
//	}
//
//	bool CEngine::initialize()
//	{
//		bool bValue = false;
////		CMemoryMgr::initialize();
////		FMemory::Malloc(65535, 4);
////		mbRunning = true;
////		mpThreadPools = WL_NEW(CThreadPool, Thread);
//
////#ifdef _WINDOWS
////		bValue = CWinPlatform::initialize();
////#endif
////		commitCommand(commandType::StopDraw);
////		bValue &= CResourceMgr::initialize() &&
////				  CGuiEngine::initialize() &&
////				  CScriptEngine::initialize() &&
////				  CSoundEngine::initialize() &&
////				  CEntityMgr::initialize() &&
////				  CLightEngine::initialize() &&
////				  CGraphicEngine::initialize() && 
////				  CInputMgr::initialize() &&
////				  CTimerEngine::initialize() &&
////				  CFontEngine::initialize();
////
////		CGCObjectMgr::createInstance();
////		
////		commitCommand(commandType::RecoverDraw);
////		registerController(WL_NEW(CSceneController, Controller)(mpScene), 1);
////		registerController(WL_NEW(CGameController, Controller), 2);
////		registerController(WL_NEW(CCameraController, Controller)(getMainCameraEntity()), 3);
////#ifdef TERRAIN_EDIT
////		//auto pInstance = CVoxelTerrainEditor::createInstance();
////		//if (nullptr != pInstance)
////		//{
////		registerController(WL_NEW(CVoxelTerrainEditor, Controller), -1);
////		//}
////#else
////		mpVoxelMgr = WL_NEW(CVoxelMgr, Manager)();
////#endif
////		CGameplayTagsManager::initializeManager();
////		CBehaviorTreeMgr::regBTreeToScrpit();
////
////		doFile("script/GameApp.lua", "ScriptStart");
//		return bValue;
//	}
//
//	void CEngine::unInitialize()
//	{
////		doString("ScriptEnd()");
////		CGameplayTagsManager::unInitializeManager();
////		CBehaviorTreeMgr::unRegBTreeToScrpit();
////		WL_DECREASE(mpVoxelMgr);
////		mbRunning = false;
////#ifdef _WINDOWS
////		CWinPlatform::unInitialize();
////#endif
////		CGraphicEngine::unInitialize();
////		CGuiEngine::unInitialize();
////		CScriptEngine::unInitialize();
////		CSoundEngine::unInitialize();
////		CLightEngine::unInitialize();
////		CResourceMgr::unInitialize();
////		CEntityMgr::unInitialize();
////		CInputMgr::unInitialize();
////		CTimerEngine::unInitialize();
////		CFontEngine::unInitialize();
////		WL_DELETE(mpScene, Scene);
////		WL_DELETE(mpThreadPools, Thread);
////		CMemoryMgr::unInitialize();
////		CGCObjectMgr::destory();
//	}
//
//	bool CEngine::isRun()
//	{
//		return mbRunning;
//	}
//
//	void CEngine::exitGame()
//	{
//#ifdef  _WINDOWS
//		SendMessage(mhMainWnd, WM_DESTROY, 0, 0);
//#endif
//	}
//
//	CScene* CEngine::createScene()
//	{
//		WL_DELETE(mpNextScene, Scene);
//		mpNextScene = WL_NEW(CScene, Scene);
//		return mpNextScene;
//	}
//
//	CScene* CEngine::createScene(const std::string& szFile)
//	{
//		CScene* pScene = createScene();
//		std::string sceneFile = szFile;
//		if (!pScene->initialise(sceneFile.c_str()))
//		{
//			WL_DELETE(pScene, Scene);
//		}
//		return pScene;
//	}
//
//	CGameplayTagsManager* CEngine::getGameplayTagsMgr()
//	{
//		return CGameplayTagsManager::getSinglePtr();
//	}
//
//
//	void CEngine::gotoScene(CScene* pScene)
//	{
//		mpNextScene = pScene;
//	}
//
//	CScene* CEngine::getCurrentScenePtr() const
//	{
//		return mpScene;
//	}
//
//	CScene* CEngine::getNextScenePtr() const
//	{
//		return mpNextScene;
//	}
//
//	CCameraEntity* CEngine::getMainCameraEntity()
//	{
//		if (nullptr != mpScene)
//		{
//			return mpScene->getMainCamera();
//		}
//		else
//		{
//			return nullptr;
//		}
//	}
//
//	void CEngine::renderWorld()
//	{
//		static UINT32 dTime = GetTickCount();
//		UINT32 offTime = GetTickCount(); 
//		procesCommandBuf();
//		if (nullptr != mpNextScene)
//		{
//			m_SceneMutex.lock();
//			WL_DELETE(mpScene, Scene);
//			mpScene = mpNextScene;
//			m_SceneMutex.unlock();
//			mpNextScene = nullptr;
//		}
//		if (nullptr != mpScene)
//		{
//			mpScene->draw(offTime - dTime);
//		}
//		dTime = offTime;
//	}
//
//	void CEngine::setVoxelMgr(CVoxelMgr* pMgr)
//	{
//		if (nullptr != pMgr)
//		{
//			WL_INCREASE(pMgr);
//			WL_DECREASE(mpVoxelMgr);
//			mpVoxelMgr = pMgr;
//		}
//	}
//
//	CVoxelMgr* CEngine::getVoxelMgr() const
//	{
//		assert(mpVoxelMgr);
//		return mpVoxelMgr;
//	}
//
//	void CEngine::executeCmd(const std::string& cmd, void* pParams)
//	{
//		if( "Fps" == cmd)
//		{
//			auto pScene = getCurrentScenePtr();
//			if (nullptr != pScene)
//			{
//				bool bShow = *static_cast<bool*>(pParams);
//				if (bShow)
//				{
//					auto& pGuis = pScene->getGuis();
//					for( auto& gui : pGuis )
//					{
//						if( gui->getEntityName() == "FPS")
//						{
//							gui->setVisible(bShow);
//							return;
//						}
//					}
//					auto fFPSWnd = WL_NEW(CGuiEntity, Entity);
//					fFPSWnd->setEntityName("FPS");
//					fFPSWnd->initialise((R"(resource\Gui\FPSWnd.gui)"));
//					pScene->addEntity(fFPSWnd);
//					fFPSWnd->setVisible(true);
//					fFPSWnd->setText(1, "FPS:0");
//					WL_DECREASE(fFPSWnd);
//				}
//			}
//		}
//	}

}



