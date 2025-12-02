#pragma once
#include "Common.h"
#include "UnrealMemory.h"

#define UE_CONTAINER_SLACK_GROWTH_FACTOR_DENOMINATOR 8
#define UE_CONTAINER_SLACK_GROWTH_FACTOR_NUMERATOR 3
namespace WL
{
	template <typename SizeType>
	FORCEINLINE SizeType DefaultCalculateSlackGrow(SizeType NewMax, SizeType CurrentMax, SIZE_T BytesPerElement, bool bAllowQuantize, uint32 Alignment = DEFAULT_ALIGNMENT)
	{
#if AGGRESSIVE_MEMORY_SAVING
		const SIZE_T FirstGrow = 1;
		const SIZE_T ConstantGrow = 0;
#else
		const SIZE_T FirstGrow = 4;
		const SIZE_T ConstantGrow = 16;
#endif

		SizeType Retval;
	//	checkSlow(NewMax > CurrentMax && NewMax > 0);

		SIZE_T Grow = FirstGrow; // this is the amount for the first alloc
#if 1
		if (CurrentMax)
		{
			// Allocate slack for the array proportional to its size.
			Grow = SIZE_T(NewMax) + UE_CONTAINER_SLACK_GROWTH_FACTOR_NUMERATOR * SIZE_T(NewMax) / UE_CONTAINER_SLACK_GROWTH_FACTOR_DENOMINATOR + ConstantGrow;
		}
		else if (SIZE_T(NewMax) > Grow)
		{
			Grow = SIZE_T(NewMax);
		}
#else

#if CONTAINER_INITIAL_ALLOC_ZERO_SLACK
		if (CurrentMax)
		{
			// Allocate slack for the array proportional to its size.
			Grow = SIZE_T(NewMax) + UE_CONTAINER_SLACK_GROWTH_FACTOR_NUMERATOR * SIZE_T(NewMax) / UE_CONTAINER_SLACK_GROWTH_FACTOR_DENOMINATOR + ConstantGrow;
		}
		else if (SIZE_T(NewMax) > Grow)
		{
			Grow = SIZE_T(NewMax);
		}
#else
		if (CurrentMax || SIZE_T(NewMax) > Grow)
		{
			// Allocate slack for the array proportional to its size.
			Grow = SIZE_T(NewMax) + UE_CONTAINER_SLACK_GROWTH_FACTOR_NUMERATOR * SIZE_T(NewMax) / UE_CONTAINER_SLACK_GROWTH_FACTOR_DENOMINATOR + ConstantGrow;
		}
#endif
#endif
		if (bAllowQuantize)
		{
			Retval = (SizeType)(FMemory::QuantizeSize(Grow * BytesPerElement, Alignment) / BytesPerElement);
		}
		else
		{
			Retval = (SizeType)Grow;
		}
		// NumElements and MaxElements are stored in 32 bit signed integers so we must be careful not to overflow here.
		if (NewMax > Retval)
		{
			Retval = TNumericLimits<SizeType>::Max();
		}

		return Retval;
	}


	/** The indirect allocation policy always allocates the elements indirectly. */
	template<uint32 Alignment = DEFAULT_ALIGNMENT>
	class TAlignedHeapAllocator
	{
	public:
		using SizeType = int32;
		enum { NeedsElementType = false };
		enum { RequireRangeCheck = true };

	};


	template <typename AllocatorType>
	struct TAllocatorTraitsBase
	{
		enum { IsZeroConstruct = false };
		enum { SupportsFreezeMemoryImage = false };
		enum { SupportsElementAlignment = false };
		enum { SupportsSlackTracking = false };
	};

	template <typename AllocatorType>
	struct TAllocatorTraits : TAllocatorTraitsBase<AllocatorType>
	{
	};

	template <uint32 Alignment>
	struct TAllocatorTraits<TAlignedHeapAllocator<Alignment>> : TAllocatorTraitsBase<TAlignedHeapAllocator<Alignment>>
	{
		enum { IsZeroConstruct = true };
		enum { SupportsSlackTracking = true };
	};

	[[noreturn]] void OnInvalidSizedHeapAllocatorNum(INT32 IndexSize, INT64 NewNum, SIZE_T NumBytesPerElement);

	/** A type which is used to represent a script type that is unknown at compile time. */
	struct FScriptContainerElement
	{
	};


	template <int IndexSize>
	struct TBitsToSizeType
	{
		// Fabricate a compile-time false result that's still dependent on the template parameter
		static_assert(IndexSize == IndexSize + 1, "Unsupported allocator index size.");
	};

	template <> struct TBitsToSizeType<8> { using Type = INT8; };
	template <> struct TBitsToSizeType<16> { using Type = INT16; };
	template <> struct TBitsToSizeType<32> { using Type = INT32; };
	template <> struct TBitsToSizeType<64> { using Type = INT64; };

	/** The indirect allocation policy always allocates the elements indirectly. */
	template <int IndexSize, typename BaseMallocType = FMemory>
	class TSizedHeapAllocator
	{
	public:
		using SizeType = typename TBitsToSizeType<IndexSize>::Type;
		enum { NeedsElementType = false };
		enum { RequireRangeCheck = true };

	private:
		using USizeType = std::make_unsigned_t<SizeType>;

	public:
		class ForAnyElementType
		{
			template <int, typename>
			friend class TSizedHeapAllocator;

		public:
			/** Default constructor. */
			ForAnyElementType()
			: Data(nullptr)
			{
			}
			
			template <typename OtherAllocator>
			FORCEINLINE void MoveToEmptyFromOtherAllocator(typename OtherAllocator::ForAnyElementType& Other)
			{
				checkSlow((void*)this != (void*)&Other);

				if (Data)
				{
#if UE_ENABLE_ARRAY_SLACK_TRACKING
					FArraySlackTrackingHeader::Free(Data);
#else
					BaseMallocType::Free(Data);
#endif
				}

				Data = Other.Data;
				Other.Data = nullptr;
			}

			FORCEINLINE void MoveToEmpty(ForAnyElementType& Other)
			{
				this->MoveToEmptyFromOtherAllocator<TSizedHeapAllocator>(Other);
			}
			FORCEINLINE ~ForAnyElementType()
			{
				if (Data)
				{
#if UE_ENABLE_ARRAY_SLACK_TRACKING
					FArraySlackTrackingHeader::Free(Data);
#else
					BaseMallocType::Free(Data);
#endif
				}
			}
			// FContainerAllocatorInterface
			FORCEINLINE FScriptContainerElement* GetAllocation() const
			{
				return Data;
			}

			void ResizeAllocation(SizeType CurrentNum, SizeType NewMax, SIZE_T NumBytesPerElement)
			{
				// Avoid calling FMemory::Realloc( nullptr, 0 ) as ANSI C mandates returning a valid pointer which is not what we want.
				if (Data || NewMax)
				{
					static_assert(sizeof(SizeType) <= sizeof(SIZE_T), "SIZE_T is expected to handle all possible sizes");

					// Check for under/overflow
					bool bInvalidResize = NewMax < 0 || NumBytesPerElement < 1 || NumBytesPerElement >(SIZE_T)MAX_int32;
					if constexpr (sizeof(SizeType) == sizeof(SIZE_T))
					{
						bInvalidResize = bInvalidResize || (SIZE_T)(USizeType)NewMax > (SIZE_T)TNumericLimits<SizeType>::Max() / NumBytesPerElement;
					}
					if (UNLIKELY(bInvalidResize))
					{
						OnInvalidSizedHeapAllocatorNum(IndexSize, NewMax, NumBytesPerElement);
					}

#if UE_ENABLE_ARRAY_SLACK_TRACKING
					Data = (FScriptContainerElement*)FArraySlackTrackingHeader::Realloc(Data, NewMax, NumBytesPerElement, 0);
#else
					Data = (FScriptContainerElement*)BaseMallocType::Realloc(Data, NewMax * NumBytesPerElement);
#endif
				}
			}
			void ResizeAllocation(SizeType CurrentNum, SizeType NewMax, SIZE_T NumBytesPerElement, uint32 AlignmentOfElement)
			{
				// Avoid calling FMemory::Realloc( nullptr, 0 ) as ANSI C mandates returning a valid pointer which is not what we want.
				if (Data || NewMax)
				{
					static_assert(sizeof(SizeType) <= sizeof(SIZE_T), "SIZE_T is expected to handle all possible sizes");

					// Check for under/overflow
					bool bInvalidResize = NewMax < 0 || NumBytesPerElement < 1 || NumBytesPerElement >(SIZE_T)MAX_int32;
					if constexpr (sizeof(SizeType) == sizeof(SIZE_T))
					{
						bInvalidResize = bInvalidResize || ((SIZE_T)(USizeType)NewMax > (SIZE_T)TNumericLimits<SizeType>::Max() / NumBytesPerElement);
					}
					if (UNLIKELY(bInvalidResize))
					{
						OnInvalidSizedHeapAllocatorNum(IndexSize, NewMax, NumBytesPerElement);
					}

#if UE_ENABLE_ARRAY_SLACK_TRACKING
					Data = (FScriptContainerElement*)FArraySlackTrackingHeader::Realloc(Data, NewMax, NumBytesPerElement, AlignmentOfElement > alignof(FArraySlackTrackingHeader) ? AlignmentOfElement : alignof(FArraySlackTrackingHeader));
#else
					Data = (FScriptContainerElement*)BaseMallocType::Realloc(Data, NewMax * NumBytesPerElement, AlignmentOfElement);
#endif
				}
			}
			SizeType GetInitialCapacity() const
			{
				return 0;
			}
			FORCEINLINE SizeType CalculateSlackGrow(SizeType NewMax, SizeType CurrentMax, SIZE_T NumBytesPerElement) const
			{
				return DefaultCalculateSlackGrow(NewMax, CurrentMax, NumBytesPerElement, true);
			}
			FORCEINLINE SizeType CalculateSlackGrow(SizeType NewMax, SizeType CurrentMax, SIZE_T NumBytesPerElement, uint32 AlignmentOfElement) const
			{
				return DefaultCalculateSlackGrow(NewMax, CurrentMax, NumBytesPerElement, true, (uint32)AlignmentOfElement);
			}

		private:
			FScriptContainerElement* Data = nullptr;
		};
		template<typename ElementType>
		class ForElementType : public ForAnyElementType
		{
		public:
			/** Default constructor. */
			ForElementType() = default;

			__forceinline ElementType* GetAllocation() const
			{
				return (ElementType*)ForAnyElementType::GetAllocation();
			}
		};
	};


	template <int IndexSize> 
	class TSizedDefaultAllocator : public TSizedHeapAllocator<IndexSize> 
	{
	public: 
		typedef TSizedHeapAllocator<IndexSize> Typedef; 
	};

	template<int IndexSize> class TSizedDefaultAllocator;
	using FDefaultAllocator = TSizedDefaultAllocator<32>;
}