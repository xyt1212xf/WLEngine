#pragma once
#include "NumericLimits.h"

namespace WL
{

	/** Which allocator is being used */
	enum EMemoryAllocatorToUse
	{
		Ansi, // Default C allocator
		Stomp, // Allocator to check for memory stomping
		TBB, // Thread Building Blocks malloc
		Jemalloc, // Linux/FreeBSD malloc
		Binned, // Older binned malloc
		Binned2, // Newer binned malloc
		Binned3, // Newer VM-based binned malloc, 64 bit only
		Platform, // Custom platform specific allocator
		Mimalloc, // mimalloc
		Libpas, // libpas
	};
	static EMemoryAllocatorToUse AllocatorToUse;
	class FMalloc;
	struct FGenericPlatformMemory
	{
		static FMalloc* BaseAllocator();

		[[noreturn]] static void OnOutOfMemory(uint64 Size, uint32 Alignment);
	};
}