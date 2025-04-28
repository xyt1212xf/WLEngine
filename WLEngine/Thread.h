#pragma once
#include "NonCopyable.h"
#include "PlatformThread.h"

namespace WL
{
	class CThread : CNonCopyable
	{
		typedef CPlatformThread::ThreadID ThreadID;
	public:
		CThread();
		~CThread();
		static ThreadID getCurrentThreadID()
		{
			return CPlatformThread::getCurrentThreadID();
		}

		static bool equalsCurrentThreadID(ThreadID thread)
		{
			return getCurrentThreadID() == thread; 
		}
		static bool CurrentThreadIsMainThread()
		{
			return equalsCurrentThreadID(mainThreadId); 
		}
		static void sleep(double seconds);

		// Stores the pointer only
		void setName(const char* name)
		{
			m_Name = name; 
		}
		
	private:
		static ThreadID mainThreadId;
		std::string m_Name = "";
		CPlatformThread	m_Thread;
	};
}