#pragma once
#include "optick.h"
#include "TSingle.h"
#include "GraphicEngine.h"
#include "InputMgr.h"
#include "GuiEngine.h"
#include "ScriptEngine.h"
#include "SoundEngine.h"
#include "LightEngine.h"
#include "TimerEngine.h"
#include "FontEngine.h"
#include "ResourceMgr.h"
#include "Vector.h"
#include "Matrix.h"
#include "Quaternion.h"
#include "ThreadPool.h"
#ifdef  _WINDOWS
#include "Win.h"
#endif
#include "MemoryMgr.h"
#include "EntityMgr.h"
#include "TimerClock.h"
#include "GameplayTagsManager.h"
#include "DefMemory.h"
namespace WL
{
	enum eRT
	{
		DeviceRT = 0,
		LightRT = 1,
		FrontRT = 2,
	};
	class CVoxelMgr;
	class CScene;

	class CEngine : public CMemoryMgr,
					public CGraphicEngine,
					public CGuiEngine,
					public CScriptEngine,
					public CSoundEngine,
					public CLightEngine,
					public CResourceMgr,
					public CEntityMgr,
					public CInputMgr,
					public CTimerEngine,
					public CFontEngine,
					public TSingle<CEngine>,
#ifdef  _WINDOWS
					public CWinPlatform
#endif //  _WINDOWS
	{
	public:
		CEngine();
		virtual ~CEngine();
		void update(UINT32 dTime);
		bool initialize();
		void unInitialize();
		bool isRun();
		void exitGame();
		CScene* createScene();
		CScene* createScene(const std::string& szFile);
		void gotoScene(CScene* pScene);
		CScene* getCurrentScenePtr() const;
		CScene* getNextScenePtr() const;
		CCameraEntity* getMainCameraEntity();
		void renderWorld();

		template<class F, class... Args>
		auto threadJoin(F&& f, Args&&... args);// ->std::future<typename std::result_of<F(Args...)>::type>;

		template<class F, class... Args>
		void threadDetach(F&& f, Args&&... args);// ->std::future<typename std::result_of<F(Args...)>::type>;		
		
		void setVoxelMgr(CVoxelMgr* pMgr);
		CVoxelMgr* getVoxelMgr() const;

		CGameplayTagsManager* getGameplayTagsMgr();

		void executeCmd(const std::string& cmd, void* pParams);

	private:
		virtual void _update(UINT32) {}

	private:
		CMutex m_SceneMutex;
		CScene* mpScene = nullptr;
		CScene* mpNextScene = nullptr;
		CThreadPool* mpThreadPools = nullptr;
		CVoxelMgr* mpVoxelMgr = nullptr;
		bool mbRunning = false;
	};
	inline CEngine* GEngine = nullptr;

	template<class F, class... Args>
	auto CEngine::threadJoin(F&& f, Args&&... args)//->std::future<typename std::result_of<F(Args...)>::type>
	{
		return mpThreadPools->enqueueJoin(std::forward<F>(f), std::forward<Args>(args)...);
	}

	template<class F, class... Args>
	void CEngine::threadDetach(F&& f, Args&&... args)//->std::future<typename std::result_of<F(Args...)>::type>
	{
#ifdef _DEBUG
		if (nullptr != mpThreadPools)
		{
			mpThreadPools->enqueueDetach(std::forward<F>(f), std::forward<Args>(args)...);
		}
#else
		mpThreadPools->enqueueDetach(std::forward<F>(f), std::forward<Args>(args)...);
#endif
	}
}


#define Dev		WL::GEngine->getDevice()
#define GraphicDev	WL::GEngine->getGraphicsDevice();



