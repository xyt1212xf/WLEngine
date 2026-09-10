#include "MemoryProfiler.h"
#include "MemoryMgr.h"
namespace WL
{
	SAllocationRootReference::SAllocationRootReference(SProfilerAllocationHeader* header, size_t size, const char* areaName, const char* objectName)
	: m_AccumulatedSize(size)
	, m_AreaName(areaName)
	{
		setObjectName(objectName);
		header->rootReference = this;
	}

	SAllocationRootReference::~SAllocationRootReference()
	{
		AssertMsg(m_AccumulatedSize == 0, "some allocations have not been deleted, yet reference count is 0. This should not happen");
		//GetMemoryProfiler()->UnregisterRootAllocation(this);
	}

	void SAllocationRootReference::setObjectName(const char* objectName)
	{
		m_ObjectName = objectName ? objectName : "";
	}

	//////////////////////////////////////////////////////////////////////////
	CMemoryProfiler* CMemoryProfiler::s_MemoryProfiler = nullptr;

	CMemoryProfiler::CMemoryProfiler()
	{

	}

	CMemoryProfiler::~CMemoryProfiler()
	{

	}

	void CMemoryProfiler::staticInitialize()
	{
		// This delayed initialization is necessary to avoid recursion, when TLS variables that are used
		// by the profiler, are themselves managed by the memory manager.
		CMemoryProfiler* temp = new (CMemoryMgr::lowLevelAllocate(sizeof(CMemoryProfiler))) CMemoryProfiler();
		temp->allocateStructs();
		s_MemoryProfiler = temp;
	}

	void CMemoryProfiler::staticDestroy()
	{

	}

	int CMemoryProfiler::getHeaderSize()
	{
		return sizeof(SProfilerAllocationHeader);
	}

	void CMemoryProfiler::allocateStructs()
	{
		//m_RootReferenceStack = WL_NEW(RootReferenceStack, kMemMemoryProfiler);
		//RootReferenceStack* stack = m_RootReferenceStack;

		//stack->rootStackSize = 20;
		//stack->rootStack = (AllocationRootReference**)WL_MALLOC(kMemMemoryProfiler, stack->rootStackSize * sizeof(AllocationRootReference*));

		//m_CurrentRootReference = &(stack->rootStack[0]);
		//*m_CurrentRootReference = NULL;
	}

}