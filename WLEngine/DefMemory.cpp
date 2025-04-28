#include <stdlib.h>
#include <new.h>
#include <vector>

#include "DefMemory.h"
#include "MemoryMgr.h"
#include "TLSAllocator.h"
#include "StackAllocator.h"
#include "BucketAllocator.h"
#include "DefaultAllocator.h"

//typedef DualThreadAllocator< DynamicHeapAllocator< LowLevelAllocator > > MainThreadAllocator;
typedef WL::TLSAllocator<WL::CStackAllocator> TempTLSAllocator;


void* getPreallocatedMemory(int size)
{
	const size_t numAllocators = 20; // should be configured per platform
	const size_t additionalStaticMem = sizeof(WL::CDefaultAllocator<void>) * numAllocators;

	// preallocated memory for memorymanager and allocators

	// Any use of HEAP_NEW must be accounted for in this calculation
	static const int preallocatedSize = sizeof(WL::CMemoryMgr) + sizeof(TempTLSAllocator) + sizeof(WL::CBucketAllocator) + additionalStaticMem;
	static UINT8 ALIGN_TYPE(16) g_MemoryBlockForMemoryManager[preallocatedSize];
	static UINT8* g_MemoryBlockPtr = g_MemoryBlockForMemoryManager;
	static size_t g_MemoryIndex = 0;

	size += alignUp(size, 16);
	void* ptr = g_MemoryBlockPtr;
	g_MemoryBlockPtr += size;
	// Ensure that there is enough space on the preallocated block
	if (g_MemoryBlockPtr > g_MemoryBlockForMemoryManager + preallocatedSize)
	{
		int* i = 0;
		*i = 10;
		return nullptr;
	}
	return ptr;
}

void* malloc_internal(size_t size, size_t align, WL::MemLabelRef label, int allocateOptions, const char* file, int line)
{
	return WL::GMemMgrPtr->allocate(size, align, label, allocateOptions, file, line);
}

void free_alloc_internal(void* ptr, WL::MemLabelRef label)
{
	WL::GMemMgrPtr->deallocate(ptr, label);
}

void* calloc_internal(size_t count, size_t size, int align, WL::MemLabelRef label, int allocateOptions, const char* file, int line)
{
	void* ptr = WL::GMemMgrPtr->allocate(size * count, align, label, allocateOptions, file, line);
	if (ptr) memset(ptr, 0, size * count);
	return ptr;
}

void* realloc_internal(void* ptr, size_t size, int align, WL::MemLabelRef label, int allocateOptions, const char* file, int line)
{
	return WL::GMemMgrPtr->reallocate(ptr, size, align, label, allocateOptions, file, line);
}

void free_internal(void* ptr)
{
	// used for mac, since malloc is not hooked from the start, so a number of mallocs will have passed through
	// before we wrap. This results in pointers being freed, that were not allocated with the memorymanager
	// therefore we need the alloc->Contains check()
	WL::GMemMgrPtr->deallocate(ptr);
}


//void* operator new(size_t size, WL::MemLabelIdentifier label, int align, const char* areaName, const char* objectName, const char* file, int line)
//{
//	void* pData = nullptr;
//	return pData;
//}

void* operator new(size_t size, WL::MemLabelRef label, int align, const char* file, int line)
{
	void* pData = malloc_internal(size, align, label, WL::kAllocateOptionNone, file, line);
	return pData;
}


void operator delete (void* p, WL::MemLabelRef label, int /*align*/, const char* /*areaName*/, const char* /*objectName*/, const char* /*file*/, int /*line*/)
{
	free_alloc_internal(p, label);
}

void operator delete (void* p, WL::MemLabelRef label, int /*align*/, const char* /*file*/, int /*line*/)
{
	free_alloc_internal(p, label);
}



//void* operator new (size_t size)noexcept
//{
//	return WL::GMemMgrPtr->allocate(size, WL::kDefaultMemoryAlignment, WL::MemLabelRef(WL::kMemNewDeleteId), WL::kAllocateOptionNone, "Overloaded New");
//}
//
//void* operator new[](size_t size)noexcept
//{
//	return WL::GMemMgrPtr->allocate(size, WL::kDefaultMemoryAlignment, WL::MemLabelRef(WL::kMemNewDeleteId), WL::kAllocateOptionNone, "Overloaded New");
//}
//
//void operator delete (void* p)noexcept
//{
//	WL::GMemMgrPtr->deallocate(p, WL::MemLabelRef(WL::kMemNewDeleteId));
//}
//
//void operator delete[](void* p)noexcept
//{
//	WL::GMemMgrPtr->deallocate(p, WL::MemLabelRef(WL::kMemNewDeleteId));
//}


//
//namespace WL
//{
//	inline CDefAllocPool* getGlobalPool()
//	{
//		
//		static void* buffer = malloc(sizeof(CDefAllocPool));
//#ifdef _WIN32	
//		static CDefAllocPool* ls_inst = new (buffer)CDefAllocPool("GlobalPool", nullptr);
//#else
//		static CNedAllocPool* ls_inst = new (buffer)CNedAllocPool("GlobalPool", nedgetsyspool());
//#endif
//		return ls_inst;
//	}
//
//	void* defMalloc(size_t stSize, CDefAllocPool* pPool /*= nullptr*/)
//	{
//		if (nullptr == pPool)
//		{
//			return getGlobalPool()->alloc(stSize);
//		}
//		else
//		{
//			return nullptr;
//		}
//	}
//
//	void defFree(void* pMem, size_t stSize, CDefAllocPool* pPool /*= nullptr*/)
//	{
//
//	}
//
//	void* defRealloc(void* pMem, size_t stOldSize, size_t stNewSize, CDefAllocPool* pPool /*= nullptr*/)
//	{
//		return nullptr;
//	}
//
//	void* defNew(size_t stSize, CDefAllocPool* pPool /*= nullptr*/)
//	{
//		return nullptr;
//	}
//
//	void defDelete(void* pMem, CDefAllocPool* pPool /*= nullptr*/)
//	{
//
//	}
//
//	void setAllocPoolEnabled(bool bEnabled)
//	{
//
//	}
//
//	bool allocPoolEnabled()
//	{
//		return false;
//	}
//
//	int defTrim(size_t pad, CDefAllocPool* pPool /*= nullptr*/)
//	{
//		return 0;
//	}
//
//}