#pragma once
#include "PlatformMutex.h"
namespace WL
{
	class CMutex : CNonCopyable
	{
	public:
		class CAutoLock : CNonCopyable
		{
		public:
			CAutoLock(CMutex& mutex);
			~CAutoLock();

		private:
			CMutex& m_Mutex;
		};

		// On worker threads, we do not allow any mutex lock calls (using mutex locks can cause significant performance issues).
		// Thus all code must be designed to not cause any locks on worker threads. However, in cases where we do not control
		// the code (such as Physx), we can instantiate an AutoAllowWorkerThreadLock on the stack to temporarily allow the locks
		// for certain operations.
		class CAutoAllowWorkerThreadLock : CNonCopyable
		{
		public:
			CAutoAllowWorkerThreadLock(bool allow = true);
			~CAutoAllowWorkerThreadLock();
		private:
			int m_NewRefCount = 0;
			int m_OldRefCount = 0;
		};

		CMutex();
		~CMutex();

		void lock();
		void unlock();

		// Returns true if locking succeeded
		bool tryLock();

		// Returns true if the mutex is currently locked
		bool isLocked();

		void blockUntilUnlocked();

	private:
		CPlatformMutex m_Mutex;
	};
}