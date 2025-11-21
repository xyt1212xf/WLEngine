#include "GenericPlatformMemory.h"
#include "MallocAnsi.h"

namespace WL
{
	FMalloc* FGenericPlatformMemory::BaseAllocator()
	{
		static FMalloc* Instance = nullptr;
		if (Instance != nullptr)
		{
			return Instance;
		}

		Instance = new FMallocAnsi();

		return Instance;
	}
}