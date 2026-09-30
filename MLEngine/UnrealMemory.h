#pragma once
#include "Common.h"
#include "MemoryBase.h"

namespace ML
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

	//	FORCEINLINE static  void SystemFree(void* Ptr)
	//	{
	//		//MemoryTrace_Free(uint64(Ptr), EMemoryTraceRootHeap::SystemMemory);
	//		::free(Ptr);
	//	}

		static FORCEINLINE void* Memcpy(void* Dest, const void* Src, SIZE_T Count)
		{
			return std::memcpy(Dest, Src, Count);
		}

		template< class T >
		static FORCEINLINE void Memcpy(T& Dest, const T& Src)
		{
			static_assert(!std::is_pointer_v<T>(), "For pointers use the three parameters function");
			Memcpy(&Dest, &Src, sizeof(T));
		}

		static FORCEINLINE void* Memmove(void* Dest, const void* Src, SIZE_T Count)
		{
			return std::memmove(Dest, Src, Count);
		}


	private:
		static void GCreateMalloc();
		static void* MallocExternal(SIZE_T Count, uint32 Alignment = DEFAULT_ALIGNMENT);
		static void* ReallocExternal(void* Original, SIZE_T Count, uint32 Alignment = DEFAULT_ALIGNMENT);
		static void FreeExternal(void* Original);
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

	template <typename DestinationElementType, typename SourceElementType, typename SizeType>
	FORCEINLINE void RelocateConstructItems(void* Dest, SourceElementType* Source, SizeType Count)
	{
		static_assert(!std::is_const_v<SourceElementType>,
			"RelocateConstructItems: Source cannot be const");
		static_assert(sizeof(DestinationElementType) > 0 && sizeof(SourceElementType) > 0,
			"RelocateConstructItems: incomplete types are not allowed");

		// 编译期判断是否可以进行按位搬迁。
		// 这里保守地要求源和目标类型完全相同，且类型是平凡可复制、平凡析构的。
		constexpr bool bCanBitwiseRelocate =
			std::is_same_v<DestinationElementType, SourceElementType>&&
			std::is_trivially_copyable_v<SourceElementType>&&
			std::is_trivially_destructible_v<SourceElementType>;

		if constexpr (bCanBitwiseRelocate)
		{
			// 可以按位搬迁：直接移动整块内存，不调用构造/析构。
			std::memmove(Dest, Source, sizeof(SourceElementType) * Count);
		}
		else
		{
			// 不能按位搬迁：逐个在目标内存上移动构造，然后析构源对象。
			DestinationElementType* D = static_cast<DestinationElementType*>(Dest);

			while (Count)
			{
				// 在目标未初始化内存上构造新对象（使用源对象的右值）
				::new (static_cast<void*>(D)) DestinationElementType(std::move(*Source));

				// 析构源对象
				using T = SourceElementType;
				Source->~T();

				++D;
				++Source;
				--Count;
			}
		}
	}

	template <typename ElementType, typename SizeType>
	inline void DestructItems(ElementType* Element, SizeType Count)
	{
		// 编译期判断：如果不是平凡析构，才执行循环
		if constexpr (!std::is_trivially_destructible_v<ElementType>) 
		{
			for (SizeType i = 0; i < Count; ++i) 
			{
				Element[i].~ElementType(); // 显式调用析构函数
			}
		}
	}
}

