#include "pch.h"
#include "Console.h"
#include <io.h>
#include <fcntl.h>
#include "Console.h"

namespace WL
{
	bool ShowConsole(bool bShow)
	{
		static CConsole* ls_pConsole = nullptr;
		static UINT32 ls_uConsoleShowCount = 0;

		if (bShow)
		{
			if (!ls_uConsoleShowCount)
			{
				if (!::AllocConsole())
				{
					//����̨�Ѿ���������û��ͨ��ShowConsole������������
					return false;
				}
				ls_pConsole = new CConsole;
			}
			++ls_uConsoleShowCount;
		}
		else
		{
			if (!ls_uConsoleShowCount)
				return false;

			if (!--ls_uConsoleShowCount)
			{
				delete ls_pConsole;
				::FreeConsole();
				ls_pConsole = NULL;
			}
		}
		return true;
	}


	CConsole::CConsole(void)
	{
		m_hConsoleScreen = ::CreateConsoleScreenBuffer(GENERIC_WRITE | GENERIC_READ, 0, 0, CONSOLE_TEXTMODE_BUFFER, 0);

		m_hConsoleInput = ::GetStdHandle(STD_INPUT_HANDLE);
		m_hConsoleOutput = ::GetStdHandle(STD_OUTPUT_HANDLE);
		m_hConsoleError = ::GetStdHandle(STD_ERROR_HANDLE);

		FILE* file;

		file = _fdopen(_open_osfhandle((size_t)m_hConsoleOutput, _O_TEXT), "w");
		setvbuf(file, NULL, _IONBF, 0);
		m_fdOldStdOut = *stdout;
		*stdout = *file;

		file = _fdopen(_open_osfhandle((size_t)m_hConsoleError, _O_TEXT), "w");
		setvbuf(file, NULL, _IONBF, 0);
		m_fdOldStdErr = *stderr;
		*stderr = *file;

		file = _fdopen(_open_osfhandle((size_t)m_hConsoleInput, _O_TEXT), "r");
		//���ǲ���ʹ��IONBF�����Ȼdb���Ե�ʱ����������������
		//Ver(!setvbuf(file,NULL,_IONBF, 0));
		setvbuf(file, NULL, _IOFBF, 4096);
		m_fdOldStdIn = *stdin;


		*stdin = *file;

		::SetConsoleTitle(GetCommandLine());
		::SetConsoleMode(m_hConsoleInput, ENABLE_ECHO_INPUT | ENABLE_PROCESSED_INPUT | ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT);
	}

	CConsole::~CConsole(void)
	{
		::CloseHandle(m_hConsoleInput);
		::CloseHandle(m_hConsoleError);
		::CloseHandle(m_hConsoleOutput);
		::CloseHandle(m_hConsoleScreen);

		*stdout = m_fdOldStdOut;
		*stdin = m_fdOldStdIn;
		*stderr = m_fdOldStdErr;
	}
}