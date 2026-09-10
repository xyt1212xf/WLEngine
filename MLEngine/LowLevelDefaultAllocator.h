#pragma once
namespace WL	
{
	class CLowLevelAllocator
	{
	public:
		static void* malloc(size_t size);
		static void* realloc(void* ptr, size_t size, size_t oldSize);
		static void  free(void* ptr, size_t oldSize);
	};
}