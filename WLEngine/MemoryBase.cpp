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

}

