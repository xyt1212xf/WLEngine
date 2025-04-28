#include "LowLevelDefaultAllocator.h"
#include "MemoryMgr.h"
namespace WL
{
	void* CLowLevelAllocator::malloc(size_t size) 
	{
		return CMemoryMgr::lowLevelAllocate(size);
	}
	void* CLowLevelAllocator::realloc(void* ptr, size_t size, size_t oldSize)
	{
		return CMemoryMgr::lowLevelReallocate(ptr, size, oldSize);
	}
	void  CLowLevelAllocator::free(void* ptr, size_t oldSize)
	{
		CMemoryMgr::lowLevelFree(ptr, oldSize);
	}

}