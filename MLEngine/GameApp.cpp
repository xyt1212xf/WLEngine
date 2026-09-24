#include <math.h>
#include "GameApp.h"
//#include "tinyxml.h"
//#include "Scene.h"
//#include "MathLib.h"
//#include "Malloc.h"
//#include "TimerClock.h"
#include "Array.h"

#ifdef _DEBUG
#include "TestGameFunc.h"
#endif
namespace ML
{
	CGameApp::CGameApp()
	{
		ML::TArray<INT32> a;
		a.Add(3);
	}

	CGameApp::~CGameApp()
	{
	//	destoryEngine();
	}

	bool CGameApp::entry(SWindowConfig& config)
	{
		do
		{
			if (CEngine::createInstance())
			{
				CWinPlatform& platform = GEngine->GetPlatform();
				if (!(platform.initMainWindow(config) &&
					  platform.initialize()) )
				{
					continue;
				}
				if (!GEngine->Initialise(config))
				{
					continue;
				}
			}
			else
			{
				continue;
			}
			run();
		} while (false);
		return true;
	}

	bool CGameApp::destoryEngine()
	{
		//if (nullptr != mpEngine)
		//{
		//	mpEngine->unInitialize();
		//	mpEngine->destory();
		//}
		return true;
	}

	void CGameApp::run()
	{
		static UINT32 nNowTime = GetTickCount();
		MSG msg = { 0 };
		while (msg.message != WM_QUIT)
		{
			//OPTICK_FRAME("MainThread");
			if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			//UINT32 dTime = ::GetTickCount();
			//UINT32 offTime = dTime - nNowTime;
			//auto pLua = GEngine->getLuaState();
			//if (offTime >= 16)
			//{
			//	lua_getglobal(pLua, "tick");
			//	lua_pcall(pLua, 0, 0, 0);
			//	mpEngine->update(offTime);
			//	nNowTime = dTime;
			//}
		}
	}

}