#include "MallocMimalloc.h"
#include "WindowsPlatformMemory.h"
namespace WL
{

	FMallocMimalloc::FMallocMimalloc()
	{
		FPlatformMemory::MiMallocInit();
	}

	void* FMallocMimalloc::Malloc(SIZE_T Size, uint32 Alignment)
	{
		void* Result = TryMalloc(Size, Alignment);

		if (Result == nullptr && Size)
		{
			OutOfMemory(Size, Alignment);
		}

		return Result;
	}

	void* FMallocMimalloc::TryMalloc(SIZE_T Size, uint32 Alignment)
	{
		void* NewPtr = MiMallocVirtualAlloc(0, Size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		return NewPtr;
	}

	void* FMallocMimalloc::Realloc(void* Ptr, SIZE_T NewSize, uint32 Alignment)
	{
		return nullptr;
	}

	void* FMallocMimalloc::TryRealloc(void* Ptr, SIZE_T NewSize, uint32 Alignment)
	{
		return nullptr;
	}

	void FMallocMimalloc::Free(void* Ptr)
	{

	}

}