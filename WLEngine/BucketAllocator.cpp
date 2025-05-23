#include "BucketAllocator.h"
#include "Mutex.h"
#include "AllocationHeader.h"
#include "Foundation.h"
#include "DefMemory.h"
#include "MemoryMgr.h"

namespace WL
{
	CBucketAllocator::CBucketAllocator(const char* name, size_t bucketGranularity, size_t bucketsCount, size_t largeBlockSize, size_t maxLargeBlocksCount)
	: CBaseAllocator(name)
	, m_MaxBucketSize(bucketGranularity* bucketsCount)
	, m_BucketGranularity(bucketGranularity)
	, m_LargeBlockSize(largeBlockSize)
	, m_MaxLargeBlocks(maxLargeBlocksCount)
	, m_BucketGranularityBits(Foundation::highestBit(bucketGranularity))
	{
		m_Buckets.resize(bucketsCount);
		for (size_t i = 0; i < bucketsCount; ++i)
		{
			size_t bucketSize = (1 + i) * m_BucketGranularity;
			size_t realBucketSize = getRealBucketSize(bucketSize);
			m_Buckets[i] = WL_NEW_ALIGNED(SBuckets(bucketSize, realBucketSize), MemLabelRef(kMemDefaultId), 16);
		}
		// Add some initial buckets
		m_LargeBlocks = reinterpret_cast<SLargeBlock*>(WL_MALLOC(MemLabelRef(kMemDefaultId), sizeof(SLargeBlock) * maxLargeBlocksCount));
		if (m_LargeBlocks == NULL || !addLargeBlock())
		{
			for (size_t i = 0; i < bucketsCount; ++i)
			{
				m_Buckets[i]->canGrow = 0;
			}
		}
	}

	CBucketAllocator::~CBucketAllocator()
	{

	}

	void* CBucketAllocator::allocate(size_t size, int align)
	{
		if (!canAllocate(size, align))
		{
			return nullptr;
		}
		void* newRealPtr = nullptr;
		SBuckets* buckets = getBucketsForSize(size);
		while (true)
		{
			newRealPtr = buckets->popBucket();
			if (newRealPtr != nullptr)
			{
				buckets->updateUsed(1);
				break;
			}

			// Fast check if we can allocate new block
			if (atomicCompareExchange(&buckets->canGrow, 0, 0))
			{
				return nullptr;
			}
			// Disallow multiple blocks creation
			int oldUsedBlocks = atomicAdd(&buckets->usedBlocksCount, 0);
			CMutex::CAutoLock lock(buckets->growMutex);
			if (oldUsedBlocks != buckets->usedBlocksCount)
			{
				continue;
			}

			if (!addMoreBuckets(buckets))
			{
				return nullptr;
			}
		}

		CAllocationHeader* newAllocHeader = CAllocationHeader::init(newRealPtr, m_AllocatorIdentifier, size, align);
		return newAllocHeader->getUserPtr();
	}

	void* CBucketAllocator::reallocate(void* p, size_t size, int align)
	{
		void* ptr = nullptr;
		return ptr;
	}

	void CBucketAllocator::deallocate(void* p)
	{

	}


	bool CBucketAllocator::addLargeBlock()
	{
		if (m_UsedLargeBlocks >= m_MaxLargeBlocks)
		{
			return false;
		}

		void* ptr = CMemoryMgr::lowLevelAllocate(m_LargeBlockSize);
		if ( nullptr == ptr)
		{
			return false;
		}

		m_LargeBlocks[m_UsedLargeBlocks].realPtr = ptr;
		m_LargeBlocks[m_UsedLargeBlocks].endPtr = static_cast<char*>(ptr) + m_LargeBlockSize;
		m_LargeBlocks[m_UsedLargeBlocks].firstBlockPtr = static_cast<char*>(alignPtr(ptr, kBlockSize));

		atomicExchange(&m_CurrentLargeBlockUsedSize, kBlockSize | m_UsedLargeBlocks); // One block is wasted because of alignment
		atomicIncrement(&m_UsedLargeBlocks);

		return true;
	}

	bool CBucketAllocator::addMoreBuckets(SBuckets* buckets)
	{
		// Add new block from preallocated memory
		int newUsedSize = atomicAdd(&m_CurrentLargeBlockUsedSize, kBlockSize);
		int largeBlockIndex = newUsedSize & 0xFF;
		if (newUsedSize < m_LargeBlockSize)
		{
			int largeBlockOffset = (newUsedSize & (~0xFF)) - kBlockSize;
			const SLargeBlock& largeBlock = m_LargeBlocks[largeBlockIndex];

			addBlockToBuckets(buckets, largeBlock.firstBlockPtr + largeBlockOffset, kBlockSize);
			return true;
		}

		// No free memory for blocks. Allocate new large block.
		CMutex::CAutoLock lock(m_NewLargeBlockMutex);

		// Check if someone added a new block already
		if (largeBlockIndex == (m_CurrentLargeBlockUsedSize & 0xFF))
		{
			if (!addLargeBlock())
			{
				atomicCompareExchange(&buckets->canGrow, 0, 1);
				return false;
			}
		}

		return true;
	}

	int CBucketAllocator::getRealBucketSize(int size)
	{
		size = CAllocationHeader::calculateNeededAllocationSize(size, kMaxAlignment);
		return alignSize(size, kMaxAlignment);
	}

	void CBucketAllocator::addBlockToBuckets(SBuckets* buckets, void* ptr, int size)
	{
		Assert(ptr == getBlockFromPtr(ptr));

		atomicAdd(&buckets->usedBlocksCount, 1);

		const int bucketsSize = buckets->bucketsSize;
		const int bucketSizeWithHeaders = getRealBucketSize(bucketsSize);
		SBlock* block = new(ptr) SBlock(bucketsSize);

		char* p = reinterpret_cast<char*>(alignPtr(block + 1, kMaxAlignment));
		char* endPtr = reinterpret_cast<char*>(ptr) + size - bucketSizeWithHeaders;
		while (p <= endPtr)
		{
			buckets->pushBucket(p);
			p += bucketSizeWithHeaders;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	SBuckets::SBuckets(int size, int realSize)
	: usedBucketsCount(0)
	, usedBlocksCount(0)
	, maxUsedBucketsCount(0)
	, bucketsSize(size)
	, realBucketsSize(realSize), canGrow(1)
	{

	}

}

