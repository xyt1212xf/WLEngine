#pragma once
#include "Allocator.h"

namespace WL
{
	template<typename InElementType, typename InAllocatorType = FDefaultAllocator>
	class TArray
	{
		template <typename OtherInElementType, typename OtherAllocator>
		friend class TArray;
	public:
		__forceinline TArray()
			: ArrayNum(0)
			, ArrayMax(AllocatorInstance.GetInitialCapacity())
		{
		}

	public:
		using SizeType = typename InAllocatorType::SizeType;
		using ElementType = InElementType;
		using AllocatorType = InAllocatorType;

		using ElementAllocatorType = std::conditional_t <
			AllocatorType::NeedsElementType,
			typename AllocatorType::template ForElementType<ElementType>,
			typename AllocatorType::ForAnyElementType>;

	protected:
		SizeType             ArrayNum;
		SizeType             ArrayMax;
		ElementAllocatorType AllocatorInstance;

	private:
		using USizeType = typename std::make_unsigned_t<SizeType>;
	};
}