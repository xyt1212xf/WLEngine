#include "MemoryBase.h"
namespace WL
{
	void* FUseSystemMallocForNew::operator new(size_t Size)
	{
	//	return FMemory::SystemMalloc(Size);
		return nullptr;
	}

	void FUseSystemMallocForNew::operator delete(void* Ptr)
	{
	//	FMemory::SystemFree(Ptr);
	//	return nullptr;
	}

	void* FUseSystemMallocForNew::operator new[](size_t Size)
	{
	//	return FMemory::SystemMalloc(Size);
		return nullptr;
	}

	void FUseSystemMallocForNew::operator delete[](void* Ptr)
	{
	//	FMemory::SystemFree(Ptr);
	//	return nullptr;
	}

	void* FMalloc::TryMalloc(SIZE_T Count, uint32 Alignment)
	{
		return Malloc(Count, Alignment);
	}

	void* FMalloc::TryRealloc(void* Original, SIZE_T Count, uint32 Alignment /*= DEFAULT_ALIGNMENT*/)
	{
		return Realloc(Original, Count, Alignment);
	}

}

