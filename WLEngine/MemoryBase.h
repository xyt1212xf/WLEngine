#pragma once
#include "NumericLimits.h"
namespace WL
{
	enum
	{
		// Default allocator alignment. If the default is specified, the allocator applies to engine rules.
		// Blocks >= 16 bytes will be 16-byte-aligned, Blocks < 16 will be 8-byte aligned. If the allocator does
		// not support allocation alignment, the alignment will be ignored.
		DEFAULT_ALIGNMENT = 0,

		// Minimum allocator alignment
		MIN_ALIGNMENT = 8,
	};
	class FUseSystemMallocForNew
	{
	public:
		void* operator new(size_t Size);

		void operator delete(void* Ptr);

		void* operator new[](size_t Size);

		void operator delete[](void* Ptr);
	};
	
	class FMalloc : public FUseSystemMallocForNew
	{
	public:
		virtual SIZE_T QuantizeSize(SIZE_T Count, uint32 Alignment)
		{
			return Count; // Default implementation has no way of determining this
		}
	};

	extern class FMalloc* GMalloc;
}