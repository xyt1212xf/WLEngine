#pragma once
#include "NonCopyable.h"
namespace WL
{
	class CPlatformThread : CNonCopyable
	{
		friend class CThread; 
	protected:
		typedef DWORD ThreadID;
		static ThreadID getCurrentThreadID()
		{
			return GetCurrentThreadId();
		}

	private:
		CPlatformThread();
		~CPlatformThread();
	};
}