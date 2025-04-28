// GameApp.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "GameApp.h"
#include "Export.h"

extern "C"
{
	int (*wlMain)(HINSTANCE hInstance, HINSTANCE h, int nShow) = nullptr;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	int ret = -1;
	HMODULE hGame = nullptr;
#ifdef _DEBUG
	hGame = LoadLibraryA("WLEngine_D.dll");
#else
	hGame = LoadLibraryA("WLEngine.dll");
#endif
	if (nullptr != hGame)
	{
		do
		{
			wlMain = (int (*)(HINSTANCE, HINSTANCE, int))GetProcAddress(hGame, "wlMain");
			if (nullptr == wlMain)
			{
				MessageBoxA(NULL, "load dll failed", "Error", MB_OK | MB_ICONWARNING);
				continue;
			}
			ret = wlMain(hInstance, hPrevInstance, nCmdShow);
			FreeLibrary(hGame);
		} while (false);
	}
	return ret;
}

