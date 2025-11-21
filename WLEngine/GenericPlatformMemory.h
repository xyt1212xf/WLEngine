#pragma once
namespace WL
{
	class FMalloc;
	struct FGenericPlatformMemory
	{
		static FMalloc* BaseAllocator();
	};
}