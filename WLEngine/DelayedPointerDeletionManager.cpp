#include "DelayedPointerDeletionManager.h"
namespace WL
{

	CDelayedPointerDeletionManager::CDelayedPointerDeletionManager(CBaseAllocator* mainAlloc, CBaseAllocator* threadAlloc)
	: m_MainThreadPendingPointers(NULL)
	, m_MainThreadPendingCount(0)
	, m_MainThreadPendingReserved(0)
	, m_HasPendingDeletes(0)
	, m_MainAlloctor(mainAlloc)
	, m_ThreadAlloctor(threadAlloc)
	{

	}

	CDelayedPointerDeletionManager::~CDelayedPointerDeletionManager()
	{
		cleanupPendingMainThreadPointers();
	}

	void CDelayedPointerDeletionManager::addPointerToMainThreadDealloc(void* ptr)
	{

	}

	void CDelayedPointerDeletionManager::cleanupPendingMainThreadPointers()
	{

	}

	void CDelayedPointerDeletionManager::deallocateLocalMemory()
	{

	}

	void CDelayedPointerDeletionManager::growPendingBuffer()
	{

	}

}