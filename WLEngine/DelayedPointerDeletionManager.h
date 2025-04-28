#pragma once
#include "BaseAllocator.h"
#include "Mutex.h"

namespace WL
{
	class CDelayedPointerDeletionManager
	{
	public:
		CDelayedPointerDeletionManager(CBaseAllocator* mainAlloc, CBaseAllocator* threadAlloc);

		~CDelayedPointerDeletionManager();

		bool hasPending() const
		{
			return m_HasPendingDeletes != 0;
		}
		void addPointerToMainThreadDealloc(void* ptr);
		void cleanupPendingMainThreadPointers();
		void deallocateLocalMemory();

	private:
		void growPendingBuffer();
		void** m_MainThreadPendingPointers;
		UINT32 m_MainThreadPendingCount;
		UINT32 m_MainThreadPendingReserved;

		volatile int m_HasPendingDeletes;
		CBaseAllocator* m_MainAlloctor;
		CBaseAllocator* m_ThreadAlloctor;
		CMutex m_MainThreadPendingPointersMutex;
	};
}