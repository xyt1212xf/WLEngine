#include "GameApp.h"
#include "tinyxml.h"
#include "Scene.h"
#include "MathLib.h"
#include "Malloc.h"
#include "TimerClock.h"
#include <math.h>

namespace WL
{
	CGameApp::CGameApp()
	{
	}

	CGameApp::~CGameApp()
	{
	//	destoryEngine();
	}

	bool CGameApp::entry(WindowConfig& config)
	{
		do
		{
			if (CEngine::createInstance())
			{
				mpEngine = GEngine;
				if (!(mpEngine->initMainWindow(config) && 
					  mpEngine->initialize()))
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
		if (nullptr != mpEngine)
		{
			mpEngine->unInitialize();
			mpEngine->destory();
		}
		return true;
	}

	void CGameApp::run()
	{
		static UINT32 nNowTime = GetTickCount();
		MSG msg = { 0 };
		while (msg.message != WM_QUIT)
		{
			OPTICK_FRAME("MainThread");
			if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			UINT32 dTime = ::GetTickCount();
			UINT32 offTime = dTime - nNowTime;
			auto pLua = GEngine->getLuaState();
			if (offTime >= 16)
			{
				lua_getglobal(pLua, "tick");
				lua_pcall(pLua, 0, 0, 0);
				mpEngine->update(offTime);
				nNowTime = dTime;
			}
		}
	}

}