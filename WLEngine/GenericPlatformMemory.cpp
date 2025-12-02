#include "GenericPlatformMemory.h"
#include "MallocMimalloc.h"

namespace WL
{
	FMalloc* FGenericPlatformMemory::BaseAllocator()
	{
		static FMalloc* Instance = nullptr;
		if (Instance != nullptr)
		{
			return Instance;
		}
		AllocatorToUse = EMemoryAllocatorToUse::Mimalloc;
		Instance = new FMallocMimalloc();

		return Instance;
	}

	void FGenericPlatformMemory::OnOutOfMemory(uint64 Size, uint32 Alignment)
	{

	}

}