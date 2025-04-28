#include "BaseAllocator.h"
#include "AtomicOps.h"
namespace WL
{
	static volatile int g_IncrementIdentifier = 0x10;

	CBaseAllocator::CBaseAllocator(const char* name)
	: mName(name)
	{
		m_AllocatorIdentifier = static_cast<UINT32>(atomicIncrement(&g_IncrementIdentifier));
	}

	CBaseAllocator::~CBaseAllocator()
	{

	}

	void CBaseAllocator::registerAllocationData(size_t allocatedSize, size_t overhead)
	{
		// @TODO: Consider atomic operations
		m_TotalAllocatedBytes += allocatedSize;
		m_BookKeepingMemoryUsage += overhead;
		if (m_TotalAllocatedBytes > m_PeakAllocatedBytes)
		{
			m_PeakAllocatedBytes = m_TotalAllocatedBytes;
		}
		m_NumAllocations++;
	}

	void CBaseAllocator::registerDeallocationData(size_t allocatedSize, size_t overhead)
	{
		m_TotalAllocatedBytes -= allocatedSize;
		m_BookKeepingMemoryUsage -= overhead;
		m_NumAllocations--;
	}

}