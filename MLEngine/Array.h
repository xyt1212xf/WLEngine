#pragma once
#include "Allocator.h"
#include "UnrealTemplate.h"
#include "NumericLimits.h"
#include "ContainerAllocationPolicies.h"

namespace WL
{
	[[noreturn]] void OnInvalidArrayNum(unsigned long long NewNum)
	{
		for (;;);
	}

	// A hacky way to get the SizeType, since it's defined in the
	// (outer) allocator type, not the (inner) allocator instance type
	template <typename AllocatorInstanceType>
	using TAllocatorSizeType_T = decltype(std::declval<AllocatorInstanceType&>().GetInitialCapacity());

	// Called only when we KNOW we are going to do a realloc increasing by 1.
	// In this case, we know that max == num and can simplify things in a very 
	// hot location in the code.
	// This returns the old ArrayMax in order to save a register clobber/reload.
	template <uint32 Flags, typename AllocatorInstanceType>
	TAllocatorSizeType_T<AllocatorInstanceType> ReallocGrow1_DoAlloc_Impl(
		uint32                                       ElementSize,
		uint32                                       ElementAlignment,
		AllocatorInstanceType& AllocatorInstance,
		TAllocatorSizeType_T<AllocatorInstanceType>& ArrayMax
	)
	{

		using SizeType = TAllocatorSizeType_T<AllocatorInstanceType>;
		using USizeType = std::make_unsigned_t<SizeType>;

		const USizeType UOldMax = (USizeType)ArrayMax;
		const USizeType UNewNum = UOldMax + 1U;
		const SizeType  OldMax = (SizeType)UOldMax;
		const SizeType  NewNum = (SizeType)UNewNum;

		// This should only happen when we've underflowed or overflowed SizeType
		if (NewNum < OldMax)
		{
			OnInvalidArrayNum((unsigned long long)UNewNum);
		}

		SizeType NewMax;
		if constexpr (!!(Flags & 1)) // TAllocatorTraits<AllocatorType>::SupportsElementAlignment
		{
			NewMax = AllocatorInstance.CalculateSlackGrow(NewNum, OldMax, ElementSize, ElementAlignment);
			AllocatorInstance.ResizeAllocation(UOldMax, NewMax, ElementSize, ElementAlignment);
		}
		else
		{
			NewMax = AllocatorInstance.CalculateSlackGrow(NewNum, OldMax, ElementSize);
			AllocatorInstance.ResizeAllocation(UOldMax, NewMax, ElementSize);
		}
		ArrayMax = NewMax;
#if UE_ENABLE_ARRAY_SLACK_TRACKING
		if constexpr (!!(Flags & 2)) // TAllocatorTraits<AllocatorType>::SupportsSlackTracking
		{
			AllocatorInstance.SlackTrackerLogNum(NewNum);
		}
#endif
		return OldMax;
	}


	// Version for small sizes/alignments. This allows the parameter setup to be a single instruction
	// note the uint16 limitation allows for a single instruction setup on arm.
	template <uint32 Flags, typename AllocatorInstanceType>
	FORCENOINLINE TAllocatorSizeType_T<AllocatorInstanceType> ReallocGrow1_DoAlloc_Tiny(
		uint16 ElementSizeAndAlignment,
		AllocatorInstanceType& AllocatorInstance,
		TAllocatorSizeType_T<AllocatorInstanceType>& ArrayMax
	)
	{
		auto xx = typeid(AllocatorInstanceType).name();
		int x = Flags;
		return ReallocGrow1_DoAlloc_Impl<1, AllocatorInstanceType>(ElementSizeAndAlignment & 0xff, ElementSizeAndAlignment >> 8, AllocatorInstance, ArrayMax);
	}

	template <uint32 Flags, typename AllocatorInstanceType>
	FORCENOINLINE TAllocatorSizeType_T<AllocatorInstanceType> ReallocGrow1_DoAlloc(
		uint32                                       ElementSize,
		uint32                                       ElementAlignment,
		AllocatorInstanceType& AllocatorInstance,
		TAllocatorSizeType_T<AllocatorInstanceType>& ArrayMax
	)
	{
		return ReallocGrow1_DoAlloc_Impl<Flags, AllocatorInstanceType>(ElementSize, ElementAlignment, AllocatorInstance, ArrayMax);
	}

	// Flags are passed as a uint32 to minimize PDB impact of these generated symbols.
	//
	// 1 == TAllocatorTraits<>::SupportsElementAlignment
	// 2 == TAllocatorTraits<>::SupportsSlackTracking
	//
	// When C++20 is guaranteed, concept checks can be used instead.
	template <typename AllocatorType>
	[[nodiscard]] constexpr uint32 GetAllocatorFlags()
	{
		uint32 Result = 0;
	//	auto xx = typeid(AllocatorType).name();
		if constexpr (TAllocatorTraits<AllocatorType>::SupportsElementAlignment)
		{
			Result |= 1;
		}
		if constexpr (TAllocatorTraits<AllocatorType>::SupportsSlackTracking)
		{
			Result |= 2;
		}
		return Result;
	}



	template<typename InElementType, typename InAllocatorType = FDefaultAllocator>
	class TArray
	{
		template <typename OtherInElementType, typename OtherAllocator>
		friend class TArray;
	public:
		using SizeType = typename InAllocatorType::SizeType;
		using ElementType = InElementType;
		using AllocatorType = InAllocatorType;

		using ElementAllocatorType = std::conditional_t <
			AllocatorType::NeedsElementType,
			typename AllocatorType::template ForElementType<ElementType>,
			typename AllocatorType::ForAnyElementType>;

	public:
		TArray()
		: ArrayNum(0)
		, ArrayMax(AllocatorInstance.GetInitialCapacity())
		{
		}
		
		FORCEINLINE void CheckAddress(const ElementType* Addr) const
		{
			//checkf(Addr < GetData() || Addr >= (GetData() + ArrayMax), TEXT("Attempting to use a container element (%p) which already comes from the container being modified (%p, ArrayMax: %lld, ArrayNum: %lld, SizeofElement: %zu)!"), Addr, GetData(), (long long)ArrayMax, (long long)ArrayNum, sizeof(ElementType));
		}

		template <typename... ArgsType>
		FORCEINLINE SizeType Emplace(ArgsType&&... Args)
		{
			auto xx = typeid(AllocatorType).name();
			if (ArrayNum == ArrayMax)
			{
				if constexpr (sizeof(ElementType) <= 255 && alignof(ElementType) <= 255) // -V590 
				{
					ArrayNum = ReallocGrow1_DoAlloc_Tiny<GetAllocatorFlags<AllocatorType>()>(sizeof(ElementType) | (alignof(ElementType) << 8), AllocatorInstance, ArrayMax);
				}
				else
				{
					ArrayNum = ReallocGrow1_DoAlloc<GetAllocatorFlags<AllocatorType>()>(sizeof(ElementType), alignof(ElementType), AllocatorInstance, ArrayMax);
				}
			}
			SizeType OldArrayNum = ArrayNum;
			return OldArrayNum;
		}

		FORCEINLINE void Push(ElementType&& Item)
		{
			Add(MoveTempIfPossible(Item));
		}

		FORCEINLINE void Push(const ElementType& Item)
		{
			Add(Item);
		}
		
		FORCEINLINE SizeType Add(ElementType&& Item)
		{
			CheckAddress(&Item);
			return Emplace(MoveTempIfPossible(Item));
		}

		FORCEINLINE SizeType Add(const ElementType& Item)
		{
			CheckAddress(&Item);
			return Emplace(Item);
		}

	protected:
		SizeType             ArrayNum;
		SizeType             ArrayMax;
		ElementAllocatorType AllocatorInstance;

	private:
		using USizeType = typename std::make_unsigned_t<SizeType>;
	};
}