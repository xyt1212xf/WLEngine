#pragma once
#include "Common.h"
namespace WL
{
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
	};
}

