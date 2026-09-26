#include "UnrealMemory.h"
#include "MallocAnsi.h"

namespace ML
{
	FMalloc* GMalloc = nullptr;

	///** Helper function called on first allocation to create and initialize GMalloc */
	//static int FMemory_GCreateMalloc_ThreadUnsafe()
	//{
	//	GMalloc = FPlatformMemory::BaseAllocator();
	//	return 0;
	//}

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
		if (!GMalloc)
		{
			Alloc = MallocExternal(Count, Alignment);
		}
		else
		{
			FScopedMallocTimer Timer(0);
			Alloc = GMalloc->Malloc(Count, Alignment);
		}
		return Alloc;
	}

	void* FMemory::Realloc(void* Original, SIZE_T Count, uint32 Alignment /*= DEFAULT_ALIGNMENT*/)
	{
		void* Ptr = nullptr;

		// Depending on the underlying implementation `Malloc` here, even if `Count` is zero,
		// could do an actual allocation (it is implementation-defined what occurs). So
		// instead, since we are fine to return null with a `Count` of zero, we check for
		// that case and skip the `Malloc` call entirely.
		if (Count > 0)
		{
			Ptr = Malloc(Count, Alignment);

			if (!Ptr)
			{
				return nullptr;
			}
		}

		if (Original)
		{
			if (Ptr)
			{
				SIZE_T OriginalCount = GetAllocSize(Original);
				SIZE_T CopyCount = std::min<SIZE_T>(Count, OriginalCount); // handle the case where the new size is smaller
				Memcpy(Ptr, Original, CopyCount);
			}

			Free(Original);
		}

		return Ptr;

	}

	void FMemory::GCreateMalloc()
	{
	    if (nullptr == GMalloc)
	    {   
			GMalloc = new FMallocAnsi();
		}
	}

	void* FMemory::MallocExternal(SIZE_T Count, uint32 Alignment /*= DEFAULT_ALIGNMENT*/)
	{
		if (!GMalloc)
		{
			GCreateMalloc();
			
		}
		return GMalloc->Malloc(Count, Alignment);
	}

	void* FMemory::ReallocExternal(void* Original, SIZE_T Count, uint32 Alignment /*= DEFAULT_ALIGNMENT*/)
	{
		if (!GMalloc)
		{
			GCreateMalloc();
		}
		return GMalloc->Realloc(Original, Count, Alignment);
	}

	void FMemory::FreeExternal(void* Original)
	{
		if (!GMalloc)
		{
			GCreateMalloc();
		}
		if (Original)
		{
			GMalloc->Free(Original);
		}
	}

	//void* FMemory::Malloc(SIZE_T Count, uint32 Alignment /*= DEFAULT_ALIGNMENT*/)
	//{
	//	void* Alloc = nullptr;
	//	if (nullptr == GMalloc)
	//	{
	//		Alloc = MallocExternal(Count, Alignment);
	//	}
	//	else
	//	{

	//	}
	//	return Alloc;
	//}

	//void* FMemory::Realloc(void* Original, SIZE_T Count, uint32 Alignment /*= DEFAULT_ALIGNMENT*/)
	//{
	//	void* Ptr = nullptr;
	//	return Ptr;
	//}

	//void FMemory::GCreateMalloc()
	//{
	//	static int ThreadSafeCreationResult = FMemory_GCreateMalloc_ThreadUnsafe();
	//}

	//void* FMemory::MallocExternal(SIZE_T Count, uint32 Alignment /*= DEFAULT_ALIGNMENT*/)
	//{
	//	if (nullptr == GMalloc)
	//	{
	//		GCreateMalloc();
	//	}
	//	return GMalloc->Malloc(Count, Alignment);
	//}

}