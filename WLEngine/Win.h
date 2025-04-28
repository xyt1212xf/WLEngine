#pragma once

#include "Common.h"
#include "Vector.h"
namespace WL
{
	class CWinPlatform
	{
	public:
		CWinPlatform();
		virtual ~CWinPlatform();
		bool initialize();
		void unInitialize();
		bool initMainWindow(WindowConfig& config);
		HWND getMainWnd();
		WindowConfig* getWindowConfigPtr();
		int getMainWndWidth();
		int getMainWndHeight();
		Vec2I getMainWndSize();
		
	protected:
		HWND mhMainWnd;
		WindowConfig	mConfig;
	};
}