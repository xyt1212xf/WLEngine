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
		FreeExternal(Original);
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
		// 直接走底层 ReallocExternal(_aligned_realloc 会保留旧数据)。
		// 不能走 "Malloc + Memcpy + Free" 路线: GetAllocSize 是空壳(返回 0),
		// 会导致扩容时旧数据一个字节都拷不过去。
		if (!GMalloc)
		{
			GCreateMalloc();
		}
		return ReallocExternal(Original, Count, Alignment);
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
