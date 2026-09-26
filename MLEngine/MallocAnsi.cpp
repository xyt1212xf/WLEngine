#include "MallocAnsi.h"
namespace ML
{
	void* AnsiMalloc(SIZE_T Size, uint32 Alignment)
	{
		void* Result = _aligned_malloc( Size, Alignment );
		return Result;
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
//#if !UE_BUILD_SHIPPING
//		uint64 LocalMaxSingleAlloc = MaxSingleAlloc.Load(EMemoryOrder::Relaxed);
//		if (LocalMaxSingleAlloc != 0 && Size > LocalMaxSingleAlloc)
//		{
//			return nullptr;
//		}
//#endif

		Alignment = std::max<uint32>(Size >= 16 ? (uint32)16 : (uint32)8, Alignment);

		void* Result = AnsiMalloc(Size, Alignment);

		return Result;
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