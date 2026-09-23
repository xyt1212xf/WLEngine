// GameApp.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "MLGameApp.h"
#include "Export.h"

extern "C"
{
	int (*mlMain)(HINSTANCE hInstance, HINSTANCE h, int nShow) = nullptr;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	int ret = -1;
	HMODULE hGame = nullptr;
#ifdef _DEBUG
	hGame = LoadLibraryA("MLEngine_D.dll");
#else
	hGame = LoadLibraryA("MLEngine.dll");
#endif
	if (nullptr != hGame)
	{
		do
		{
			mlMain = (int (*)(HINSTANCE, HINSTANCE, int))GetProcAddress(hGame, "mlMain");
			if (nullptr == mlMain)
			{
				MessageBoxA(NULL, "load dll failed", "Error", MB_OK | MB_ICONWARNING);
				continue;
			}
			ret = mlMain(hInstance, hPrevInstance, nCmdShow);
			FreeLibrary(hGame);
		} while (false);
	}
	return ret;
}

