#pragma once
#include "Thread.h"
namespace WL
{
	class CThreadHelper
	{
		friend class CThread;
		friend class CPlatformThread;

	protected:
		static void sleep(double time);
		static void setThreadName(const CThread* pThread);
		static void setThreadProcessor(const CThread* pThread, int processor);
	};
}