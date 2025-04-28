#include "PlatformMutex.h"
namespace WL
{

	CPlatformMutex::CPlatformMutex()
	{
		::InitializeCriticalSectionAndSpinCount(&crit_sec, 200);
	}

	CPlatformMutex::~CPlatformMutex()
	{
		::DeleteCriticalSection(&crit_sec);
	}

	void CPlatformMutex::lock()
	{
		::EnterCriticalSection(&crit_sec);
	}

	void CPlatformMutex::unlock()
	{
		::LeaveCriticalSection(&crit_sec);
	}

	bool CPlatformMutex::tryLock()
	{
		return ::TryEnterCriticalSection(&crit_sec) ? true : false;
	}

}