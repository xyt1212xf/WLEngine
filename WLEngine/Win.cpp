#include <windowsx.h>
#include <iostream>
#include "Win.h"
#include "WLEngine.h"
#include "GameApp.h"
#include "Scene.h"
#include "MathLib.h"
#include "Foundation.h"

extern "C" 
{
	WLENGINE_API int wlMain(HINSTANCE hInstance, HINSTANCE, int nShow)
	{
		try
		{
			WL::WindowConfig config;
			config.hInstance = hInstance;
			config.szTitle = "GameApp";
			auto szContent = WL::Foundation::readJsonFile("config.json");
			auto js = json::parse(szContent.c_str());

			for (auto& item : js.items())
			{
				const auto& k = item.key();

				auto& value = item.value();
				if ("Win32config" == k)
				{
					for (const auto& [k, v] : value.items())
					{
						if ("screenWidth" == k)
						{
							config.nWidth = v.get<int>();
						}
						else if ("screenHeight" == k)
						{
							config.nHeight = v.get<int>();
						}
					}
				}
				else if ("DX11" == k)
				{
					for (const auto& [k, v] : value.items())
					{
						if ("fullScreen" == k)
						{
							config.bFullScreen = v.get<bool>();
						}
						else if ("vsync" == k)
						{
							config.bVsync = v.get<bool>();
						}
						else if ("backBufferFormat" == k)
						{
							config.backBufferFormat = v.get<int>();
						}
						else if ("depthBufferFormat" == k)
						{
							config.depthBufferFormat = v.get<int>();
						}
					}
				}
			}
			WL::CGameApp* pGameApp = new WL::CGameApp();
			if (pGameApp->entry(config))
			{
				pGameApp->destoryEngine();
			}
			SafeDelete(pGameApp);
			return 0;
		}
		catch (...)   //捕获任何异常
		{
			return -1;
		}
	}
}

namespace WL
{
	LRESULT CALLBACK MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		// Forward hwnd on because we can get messages (e.g., WM_CREATE)
		// before CreateWindow returns, and thus before m_hMainWnd is valid.
		static std::unordered_map<int, int> msgMap
		{
			{WM_MOUSEMOVE, MSG_MOUSEMOVE},

			{WM_MOUSEHWHEEL, MSG_MOUSEWHEEL},

			{WM_LBUTTONDOWN, MSG_LBUTTONDOWN},
			{WM_LBUTTONUP, MSG_LBUTTONUP},

			{WM_RBUTTONDOWN, MSG_RBUTTONDOWN},
			{WM_RBUTTONUP, MSG_RBUTTONUP},

			{WM_MBUTTONDOWN, MSG_MBUTTONDOWN},
			{WM_MBUTTONUP, MSG_MBUTTONUP},

			{WM_KEYDOWN, MSG_KEYDOWN},
			{WM_KEYUP, MSG_KEYUP},
			{WM_CHAR, MSG_CHAR},
		};

		static event msg;
		msg.bProcess = false;
		msg.message = msgMap[message];
		msg.mouseX = GET_X_LPARAM(lParam);
		msg.mouseY = GET_Y_LPARAM(lParam);
		msg.wParam = wParam;
		msg.lParam = lParam;
		GEngine->processMsg(msg);

		switch (message)
		{
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
		}
		break;
		case WM_DESTROY:
		{
			PostQuitMessage(0);
		}
		break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		return 0;
	}

	CWinPlatform::CWinPlatform()
	{

	}

	CWinPlatform::~CWinPlatform()
	{
		if (mConfig.bConsoleWin)
		{
			FreeConsole();
		}
	}

	bool CWinPlatform::initialize()
	{
		return true;
	}

	void CWinPlatform::unInitialize()
	{

	}

	bool CWinPlatform::initMainWindow(WindowConfig& config)
	{
		WNDCLASS wc;
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = MainWndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = config.hInstance;
		wc.hIcon = LoadIcon(0, IDI_APPLICATION);
		wc.hCursor = LoadCursor(0, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
		wc.lpszMenuName = 0;
		wc.lpszClassName = "WLWin32ClassName";
		if (!RegisterClass(&wc))
		{
			MessageBoxA(0, "RegisterClass Failed.", 0, 0);
			return false;
		}

		int screenWidth = GetSystemMetrics(SM_CXSCREEN);
		int screenHeight = GetSystemMetrics(SM_CYSCREEN);

		// Compute window rectangle dimensions based on requested client area dimensions.
		RECT R = { 0, 0, config.nWidth, config.nHeight };
		AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, FALSE);
		int width = R.right - R.left;
		int height = R.bottom - R.top;

		mhMainWnd = CreateWindowEx(0, "WLWin32ClassName", config.szTitle.c_str(),
			WS_OVERLAPPEDWINDOW, (screenWidth - width) / 2, (screenHeight - height) / 2, width, height, 0, 0, config.hInstance, 0);

		if (!mhMainWnd)
		{
			MessageBoxA(0, "CreateWindow Failed.", 0, 0);
			return false;
		}

		ShowWindow(mhMainWnd, SW_SHOW);
		UpdateWindow(mhMainWnd);	
		mConfig = std::move(config);
		if (mConfig.bConsoleWin)
		{
			AllocConsole();
			FILE* f;
			freopen_s(&f, "CONOUT$", "w", stdout);  // Redirect stdout to the console window
			std::cout << "Console window is open!" << std::endl;
		}
		return true;
	}

	HWND CWinPlatform::getMainWnd()
	{
		return mhMainWnd;
	}

	WindowConfig* CWinPlatform::getWindowConfigPtr()
	{
		return &mConfig;
	}

	int CWinPlatform::getMainWndWidth()
	{
		return mConfig.nWidth;
	}

	int CWinPlatform::getMainWndHeight()
	{
		return mConfig.nHeight;
	}

	Vec2I CWinPlatform::getMainWndSize()
	{
		return Vec2I(mConfig.nWidth, mConfig.nHeight);
	}

}

