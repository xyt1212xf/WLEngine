#pragma once
#include "Console.h"
#include <windows.h>
#include <stdio.h>
#include <iostream>
namespace WL
{
	extern bool ShowConsole(bool bShow);
	class CConsole
	{
	private:
		friend bool ShowConsole(bool bShow);
		
		HANDLE	m_hConsoleScreen;

		HANDLE	m_hConsoleInput;
		HANDLE	m_hConsoleOutput;
		HANDLE	m_hConsoleError;

		FILE	m_fdOldStdIn;
		FILE	m_fdOldStdOut;
		FILE	m_fdOldStdErr;

		CConsole(void);
		~CConsole(void);
	};
}