#pragma once
#include "Common.h"
#include "MemoryBase.h"

namespace WL
{
	struct FMemory
	{
		/** Some allocators can be given hints to treat allocations differently depending on how the memory is used, it's lifetime etc. */
		enum AllocationHints
		{
			None = -1,
			Default,
			Temporary,
			SmallPool,

			Max
		};

		static void Free(void* Original);
		[[nodiscard]] static SIZE_T GetAllocSize(void* Original);

		static SIZE_T QuantizeSize(SIZE_T Count, uint32 Alignment = DEFAULT_ALIGNMENT);

		static void* Malloc(SIZE_T Count, uint32 Alignment = DEFAULT_ALIGNMENT);
		static void* Realloc(void* Original, SIZE_T Count, uint32 Alignment = DEFAULT_ALIGNMENT);

		FORCEINLINE static void* SystemMalloc(SIZE_T Size)
		{
			void* Ptr = ::malloc(Size);
			//MemoryTrace_Alloc(uint64(Ptr), Size, 0, EMemoryTraceRootHeap::SystemMemory);
			return Ptr;
		}

		FORCEINLINE static  void SystemFree(void* Ptr)
		{
			//MemoryTrace_Free(uint64(Ptr), EMemoryTraceRootHeap::SystemMemory);
			::free(Ptr);
		}

	private:
		static void GCreateMalloc();
		static void* MallocExternal(SIZE_T Count, uint32 Alignment = DEFAULT_ALIGNMENT);
	};


	struct FScopedMallocTimer
	{
		FORCEINLINE FScopedMallocTimer(INT32 InIndex)
		{
		}
		FORCEINLINE ~FScopedMallocTimer()
		{
		}
		FORCEINLINE void Hit(INT32 InIndex)
		{
		}
	};


}

