#pragma once
#include "Common.h"
#define ML_NEW(type, label) new (WL::MemLabelRef(kMem##label##Id), WL::kDefaultMemoryAlignment, __FILE__, __LINE__) type
#define ML_DELETE(ptr, label) { if(ptr) delete ptr; ptr = nullptr; }

namespace ML
{

//
///**
// * Default constructs a range of items in memory.
// *
// * @param	Elements	The address of the first memory location to construct at.
// * @param	Count		The number of elements to destruct.
// */
//	template <
//		typename ElementType,
//		typename SizeType
//		UE_REQUIRES(sizeof(ElementType) > 0 && TIsZeroConstructType<ElementType>::Value) // the sizeof here should improve the error messages we get when we try to call this function with incomplete types
//	>
//	FORCEINLINE void DefaultConstructItems(void* Address, SizeType Count)
//	{
//		FMemory::Memset(Address, 0, sizeof(ElementType) * Count);
//	}
//	template <
//		typename ElementType,
//		typename SizeType
//		UE_REQUIRES(sizeof(ElementType) > 0 && !TIsZeroConstructType<ElementType>::Value) // the sizeof here should improve the error messages we get when we try to call this function with incomplete types
//	>
//	FORCENOINLINE void DefaultConstructItems(void* Address, SizeType Count)
//	{
//		ElementType* Element = (ElementType*)Address;
//		while (Count)
//		{
//			::new ((void*)Element) ElementType;
//			++Element;
//			--Count;
//		}
//	}


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
		virtual void* Malloc(SIZE_T Count, uint32 Alignment = DEFAULT_ALIGNMENT) = 0;

		virtual void* TryMalloc(SIZE_T Count, uint32 Alignment);

		virtual void* Realloc(void* Ptr, SIZE_T NewSize, uint32 Alignment) = 0;

		virtual void Free(void* Ptr) = 0;

		virtual void* TryRealloc(void* Original, SIZE_T Count, uint32 Alignment = DEFAULT_ALIGNMENT);

		virtual bool GetAllocationSize(void* Original, SIZE_T& SizeOut)
		{
			return false;
		}

		virtual SIZE_T QuantizeSize(SIZE_T Count, uint32 Alignment)
		{
			return Count; // Default implementation has no way of determining this
		}
	};

	extern class FMalloc* GMalloc;
}