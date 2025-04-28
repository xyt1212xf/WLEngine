#pragma once
#include "Common.h"

#define ENABLE_MEM_PROFILER 1 //(ENABLE_MEMORY_MANAGER && ENABLE_PROFILER)

namespace WL
{
	enum
	{
		kDefaultMemoryAlignment = 16
	};

	enum
	{
		kAllocateOptionNone = 0,						// Fatal: Show message box with out of memory error and quit application
		kAllocateOptionReturnNullIfOutOfMemory = 1		// Returns null if allocation fails (doesn't show message box)
	};

	enum MemLabelIdentifier
	{
		kMemTempLabels,
#define DO_LABEL(Name)
#define DO_TEMP_LABEL(Name) kMem##Name##Id ,
#include "AllocatorLabelNames.h"
#undef DO_TEMP_LABEL
#undef DO_LABEL

		kMemRegularLabels,

		// then add regular labels
#define DO_LABEL(Name) kMem##Name##Id ,
#define DO_TEMP_LABEL(Name)
#include "AllocatorLabelNames.h"
#undef DO_TEMP_LABEL
#undef DO_LABEL

		kMemLabelCount
	};
	struct AllocationRootReference;

	struct MemLabelId 
	{
		MemLabelId() = default;
		MemLabelId(MemLabelIdentifier id)
		{
			identifier = id;
		}
		MemLabelIdentifier identifier;
		AllocationRootReference* m_RootReference = nullptr;
	};

	typedef const MemLabelId& MemLabelRef;

	INLINE MemLabelIdentifier getLabelIdentifier(MemLabelRef label)
	{
		return label.identifier;
	}
	INLINE bool isTempLabel(MemLabelRef label)
	{
		return getLabelIdentifier(label) < kMemRegularLabels;
	}
	INLINE void* alignPtr(void* p, size_t alignment)
	{
		size_t a = alignment - 1;
		return (void*)(((size_t)p + a) & ~a);
	}

	template <typename T>
	INLINE T alignSize(T size, size_t alignment)
	{
		size_t a = alignment - 1;
		return ((size + a) & ~a);
	}


	template <typename T>
	INLINE bool doesAdditionOverflow(T a, T b)
	{
		return (std::numeric_limits<T>::max)() - a < b;
	}

	template <typename T>
	INLINE bool doesMultiplicationOverflow(T a, T b)
	{
		return b != 0 && (std::numeric_limits<T>::max)() / b < a;
	}
}