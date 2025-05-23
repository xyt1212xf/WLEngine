#pragma once
#include "BaseAllocator.h"
#include "Mutex.h"
#include "AtomicNode.h"
#include "AtomicQueue.h"

namespace WL
{
#define ATOMIC_HAS_QUEUE	2
#define USE_BUCKET_ALLOCATOR ATOMIC_HAS_QUEUE 

	struct Buckets;
	class CBucketAllocator : public CBaseAllocator
	{
	public:
		CBucketAllocator(const char* name, size_t bucketGranularity, size_t bucketsCount, size_t largeBlockSize, size_t maxLargeBlocksCount);
		virtual ~CBucketAllocator();

		/// Allocate memory block of the specified size (minimum size) and alignment.
		virtual void* allocate(size_t size, int align);
		/// Reallocate memory block at the specified pointer.
		virtual void* reallocate(void* p, size_t size, int align);
		/// Deallocate memory block.
		virtual void   deallocate(void* p);

		INLINE  bool canAllocate(size_t size, int align) const
		{
			return size <= m_MaxBucketSize && align <= kMaxAlignment; 
		}
		bool  addLargeBlock();
		bool  addMoreBuckets(Buckets* buckets);
		static int  getRealBucketSize(int size);
		INLINE Buckets* getBucketsForSize(size_t size) const
		{
			return m_Buckets[size > 0 ? ((size - 1) >> m_BucketGranularityBits) : 0]; 
		}
	private:
		struct SBlock
		{
			SBlock(int bs) : bucketSize(bs) {}

			int bucketSize = 0;
		};
		struct SLargeBlock
		{
			SLargeBlock(void* p, void* e, char* b) : realPtr(p), endPtr(e), firstBlockPtr(b) {}

			void* realPtr = nullptr;
			void* endPtr = nullptr;
			char* firstBlockPtr = nullptr;
		};

		void	addBlockToBuckets(Buckets* buckets, void* ptr, int size);
		INLINE SBlock* getBlockFromPtr(const void* ptr) const
		{
			return reinterpret_cast<SBlock*>(((size_t)ptr) & ~(kBlockSize - 1)); 
		}
	private:
		std::vector<Buckets*>    m_Buckets;                        ///< Buckets of various size.
		static const int           kMaxAlignment = 16;
		static const int           kBlockSize = 16 * 1024;
		const int                  m_BucketGranularity;
		const int                  m_BucketGranularityBits;
		const size_t               m_MaxBucketSize = 0;
		const int                  m_MaxLargeBlocks;
		const int                  m_LargeBlockSize;                 ///< Size of large memory block for all small allocations.
		SLargeBlock* m_LargeBlocks;                    ///< Large blocks of continuous memory.
		ALIGN_TYPE(4) mutable volatile int m_UsedLargeBlocks = 0;
		ALIGN_TYPE(4) volatile int m_CurrentLargeBlockUsedSize;
		CMutex                     m_NewLargeBlockMutex;
	};

	struct Buckets : public CNonCopyable
	{
		Buckets(int size, int realSize);
		INLINE void* popBucket()
		{
			return availableBuckets.pop(); 
		}
		
		INLINE void  pushBucket(void* p)
		{
			availableBuckets.push(reinterpret_cast<CAtomicNode*>(p)); 
		}
		INLINE void  updateUsed(int delta)
		{

		}
		ALIGN_TYPE(16) CAtomicStack         availableBuckets;                ///< Available buckets.
		ALIGN_TYPE(4) mutable volatile int usedBucketsCount;                ///< Count of used buckets.
		ALIGN_TYPE(4) volatile int         usedBlocksCount;                 ///< Count of used small blocks.
		ALIGN_TYPE(4) mutable volatile int maxUsedBucketsCount;             ///< Highest count of used buckets.
		ALIGN_TYPE(4) volatile int         canGrow;                         ///< True if we can add new blocks
	
		const int                          bucketsSize;                     ///< Size of this allocations.
		const int                          realBucketsSize;                 ///< Size of this bucket including AllocationHeader overhead.
		CMutex                             growMutex;                       ///< Mutex for new small block allocation


	};


}