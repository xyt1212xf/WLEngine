#pragma once
#include "NonCopyable.h"
#include "AllocatorLabels.h"
#include "AllocationHeader.h"
#include "Thread.h"

namespace WL
{
	class CBaseAllocator : public CNonCopyable
	{
	public:
		CBaseAllocator(const char* name);
		virtual ~CBaseAllocator();

		/// Return allocator name
		const char* getName() const { return mName.c_str(); }

		/// Return allocator name
		UINT32 getAllocatorIdentifier() const 
		{
			return m_AllocatorIdentifier; 
		}
		virtual size_t getAllocatedMemorySize() const
		{
			return m_TotalAllocatedBytes;
		}

		/// Allocate memory block of the specified size (minimum size) and alignment.
		virtual void* allocate(size_t size, int align) = 0;
		/// Reallocate memory block at the specified pointer.
		virtual void* reallocate(void* p, size_t size, int align) = 0;
		/// Deallocate memory block.
		virtual void   deallocate(void* p) = 0;
		
		virtual void threadInitialize([[maybe_unused]] CBaseAllocator* allocator){}
		virtual void ThreadCleanup() {}
		virtual void FrameMaintenance([[maybe_unused]] bool cleanup) {}

		/// Return true if allocator is ready. Only the TLS allocator uses this call since the TLS value for this thread might not have been initialized yet
		virtual bool   isAssigned() const { return true; }
		/// Verify allocations
		virtual bool   checkIntegrity() { return true; }

		// return the free block count for each pow2
		virtual void getFreeBlockCount([[maybe_unused]] int* freeCount, [[maybe_unused]] int size) {}
		// return the used block count for each pow2
		virtual void getUsedBlockCount([[maybe_unused]] int* usedCount, [[maybe_unused]] int size) {}

		/// Get the reserved size of the allocator (including all overhead memory allocated).
		virtual size_t GetReservedMemorySize() const
		{
			return m_TotalReservedBytes; 
		}
		
		/// Get the peak allocated size of the allocator
		virtual size_t GetPeakAllocatedMemorySize() const
		{
			return m_PeakAllocatedBytes; 
		}

		/// Get profiler headers overhead size.
		virtual size_t getBookKeepingMemorySize() const
		{
			return m_BookKeepingMemoryUsage;
		}
		// get the present number of allocations held
		virtual size_t getNumberOfAllocations() const 
		{
			return m_NumAllocations; 
		}

	protected:
		void registerAllocationData(size_t allocatedSize, size_t overhead);
		void registerDeallocationData(size_t allocatedSize, size_t overhead);

	protected:
		std::string mName = "CBaseAllocator";
		UINT32 m_AllocatorIdentifier;
		UINT32 m_NumAllocations = 0;                 ///< Allocation count
		size_t m_TotalAllocatedBytes = 0;            ///< Memory requested by the allocator
		size_t m_TotalReservedBytes = 0;             ///< All memory reserved by the allocator
		size_t m_PeakAllocatedBytes = 0;             ///< Memory requested by the allocator
		mutable size_t m_BookKeepingMemoryUsage; ///< Memory used for bookkeeping (headers etc.)
	};
}