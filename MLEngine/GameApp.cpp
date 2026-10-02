#include <math.h>
#include "GameApp.h"
//#include "tinyxml.h"
#include "World.h"
//#include "MathLib.h"
//#include "Malloc.h"
//#include "TimerClock.h"
#include "Array.h"
#include "StaticMesh.h"

//#ifdef _DEBUG
//#include "TestGameFunc.h"
//#endif
namespace ML
{
	CGameApp::CGameApp()
	{
		TArray<int32> a;
		a.Add(3);
		a.Remove(3);
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
				mpWorld = new CWorld();
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
		if (nullptr != GEngine)
		{
			GEngine->UnInitialise();
			GEngine->destory();
		}
		return true;
	}

	void CGameApp::run()
	{
		loadScene("Test");
		
		GEngine->Start();
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
			
			UINT32 dTime = ::GetTickCount();
			GEngine->Run((dTime - nNowTime)*0.001f);
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

	void CGameApp::loadScene(const std::string& name)
	{
		if (GEngine)
		{
			auto loadFunc = [](CWorld* pWorld)
				{

				};
			mpWorld->LoadAsync(name, loadFunc);
		}
		CStaticMesh Mesh;
		Mesh.AddMaterialSlot("Default");
		int32 LOD0 = Mesh.AddSourceModel();
		Mesh.GetSourceModel(LOD0)->CreateCube(Vec3F(0, 0, 0), Vec3F(1, 1, 1));
		Mesh.Build();

	}

}