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
		// _aligned_realloc 保留旧内容并按对齐重新分配; 与 AnsiMalloc 的 _aligned_malloc 配对。
		void* Result = _aligned_realloc(Ptr, NewSize, Alignment);
		return Result;
	}

	void AnsiFree(void* Ptr)
	{
		_aligned_free(Ptr); // 与 _aligned_malloc/_aligned_realloc 配对, 不能用 free
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
		void* Result = TryRealloc(Ptr, NewSize, Alignment);

		if (Result == nullptr && NewSize)
		{
			//	FPlatformMemory::OnOutOfMemory(NewSize, Alignment);
		}

		return Result;
	}

	void* FMallocAnsi::TryRealloc(void* Ptr, SIZE_T NewSize, uint32 Alignment)
	{
		// 与 TryMalloc 相同的对齐规则: 小块 8 字节, 大块 16 字节, 且不低于调用方要求
		Alignment = std::max<uint32>(NewSize >= 16 ? (uint32)16 : (uint32)8, Alignment);

		return AnsiRealloc(Ptr, NewSize, Alignment);
	}

	void FMallocAnsi::Free(void* Ptr)
	{
		AnsiFree(Ptr);
	}

}