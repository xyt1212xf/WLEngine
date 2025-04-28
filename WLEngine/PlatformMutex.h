#pragma once
#include "NonCopyable.h"
namespace WL
{
	class CPlatformMutex : public CNonCopyable
	{
		friend class CMutex;
	protected:
		CPlatformMutex();
		~CPlatformMutex();

		void lock();
		void unlock();
		bool tryLock();

	private:
		CRITICAL_SECTION crit_sec;
	};

}