#include "UnrealMemory.h"
#include "WindowsPlatformMemory.h"

namespace WL
{
	FMalloc* GMalloc = nullptr;

	/** Helper function called on first allocation to create and initialize GMalloc */
	static int FMemory_GCreateMalloc_ThreadUnsafe()
	{
		GMalloc = FPlatformMemory::BaseAllocator();
		return 0;
	}

	void FMemory::Free(void* Original)
	{
		if (nullptr == Original)
		{
			FScopedMallocTimer Timer(3);
			return;
		}
	}

	SIZE_T FMemory::GetAllocSize(void* Original)
	{
		SIZE_T Result = 0;
		return Result;
	}

	SIZE_T FMemory::QuantizeSize(SIZE_T Count, uint32 Alignment /*= DEFAULT_ALIGNMENT*/)
	{
		SIZE_T Result = 0;
		{
			if (!GMalloc)
			{
				Result = Count;
			}
			else
			{
				Result = GMalloc->QuantizeSize(Count, Alignment);
			}
		};

		return Result;
	}

	void* FMemory::Malloc(SIZE_T Count, uint32 Alignment /*= DEFAULT_ALIGNMENT*/)
	{
		void* Alloc = nullptr;
		if (nullptr == GMalloc)
		{
			Alloc = MallocExternal(Count, Alignment);
		}
		else
		{

		}
		return Alloc;
	}

	void* FMemory::Realloc(void* Original, SIZE_T Count, uint32 Alignment /*= DEFAULT_ALIGNMENT*/)
	{
		void* Ptr = nullptr;
		return Ptr;
	}

	void FMemory::GCreateMalloc()
	{
		static int ThreadSafeCreationResult = FMemory_GCreateMalloc_ThreadUnsafe();
	}

	void* FMemory::MallocExternal(SIZE_T Count, uint32 Alignment /*= DEFAULT_ALIGNMENT*/)
	{
		if (nullptr == GMalloc)
		{
			GCreateMalloc();
		}
		return GMalloc->Malloc(Count, Alignment);
	}

}