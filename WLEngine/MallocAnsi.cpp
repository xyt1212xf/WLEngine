#include "MallocAnsi.h"
namespace WL
{
	void* AnsiMalloc(SIZE_T Size, uint32 Alignment)
	{
		return nullptr;
	}

	void* AnsiRealloc(void* Ptr, SIZE_T NewSize, uint32 Alignment)
	{
		return nullptr;
	}

	void AnsiFree(void* Ptr)
	{

	}

	FMallocAnsi::FMallocAnsi()
	{

	}

	void* FMallocAnsi::Malloc(SIZE_T Size, uint32 Alignment /*= DEFAULT_ALIGNMENT*/)
	{
		void* Result = TryMalloc(Size, Alignment);

		if (Result == nullptr && Size)
		{
		//	FPlatformMemory::OnOutOfMemory(Size, Alignment);
		}

		return Result;
	}

	void* FMallocAnsi::TryMalloc(SIZE_T Size, uint32 Alignment)
	{
		return nullptr;
//#if !UE_BUILD_SHIPPING
//		uint64 LocalMaxSingleAlloc = MaxSingleAlloc.Load(EMemoryOrder::Relaxed);
//		if (LocalMaxSingleAlloc != 0 && Size > LocalMaxSingleAlloc)
//		{
//			return nullptr;
//		}
//#endif

		//Alignment = FMath::Max(Size >= 16 ? (uint32)16 : (uint32)8, Alignment);

		//void* Result = AnsiMalloc(Size, Alignment);

		//return Result;
	}

	

	void* FMallocAnsi::Realloc(void* Ptr, SIZE_T NewSize, uint32 Alignment)
	{
		return nullptr;
	}

	void* FMallocAnsi::TryRealloc(void* Ptr, SIZE_T NewSize, uint32 Alignment)
	{
		return nullptr;
	}

	void FMallocAnsi::Free(void* Ptr)
	{

	}

}