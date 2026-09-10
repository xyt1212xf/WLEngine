#pragma once
#include "BaseAllocator.h"
#include "Mutex.h"
#include "Foundation.h"
namespace WL
{
	class CBucketAllocator;

	template<class LLAllocator>
	class TDynamicHeapAllocator : public CBaseAllocator
	{
#define LARGE_BLOCK_ALLOC(_SIZE) LLAllocator::malloc (_SIZE)
#define LARGE_BLOCK_FREE(_PTR, _SIZE) LLAllocator::free (_PTR, _SIZE)
		struct SLargeAllocations
		{
			SLargeAllocations* next;
			char* allocatedPtr;
			void* returnedPtr;
			size_t            allocatedSize;
			size_t            returnedSize;
		};
		SLargeAllocations* m_FirstLargeAllocation = nullptr;

		struct SPoolElement 
		{
#ifdef _DEBUG
			SPoolElement() {}
#endif
			bool Contains(const void* ptr) const
			{
				return ptr >= memoryBase && ptr < memoryBase + memorySize;
			}
			void* tlsfPool = nullptr;
			char* memoryBase = nullptr;
			UINT32 memorySize = 0;
			UINT32 allocationCount = 0;
		};

		typedef std::list<SPoolElement> PoolList;
	private:
		// Helper function to get proper allocated size from tlsf allocator
		static size_t getTlsfAllocationSize(const CAllocationHeader* header)
		{
			size_t size = tlsf_block_size(header->getAllocationPtr());
			return header->adjustUserPtrSize(size);
		}
		SPoolElement& getActivePool(size_t size)
		{
			return getPoolList(size).front();
		}
		PoolList& getPoolList(size_t size)
		{
			return size < m_SplitLimit ? m_SmallTLSFPools : m_LargeTLSFPools;
		}
	public:
		TDynamicHeapAllocator(UINT32 poolIncrementSize, size_t splitLimit, bool useLocking, CBucketAllocator* bucketAllocator, const char* name)
		: CBaseAllocator(name)
		, m_BucketAllocator(bucketAllocator)
		, m_UseLocking(useLocking)
		{
			m_RequestedPoolSize = poolIncrementSize;
			m_SplitLimit = splitLimit;
		}
		~TDynamicHeapAllocator()
		{

		}
	

		virtual void* allocate(size_t size, int align) final
		{
			DebugAssert(align > 0 && align <= 16 * 1024 && Foundation::isPowerOfTwo(align));

			// Use lockless BucketAllocator for small allocations up to 64 bytes.
			if (m_BucketAllocator != nullptr && m_BucketAllocator->CBucketAllocator::canAllocate(size, align))
			{
				void* realPtr = m_BucketAllocator->CBucketAllocator::allocate(size, align);
				if (realPtr != nullptr)
				{
					return realPtr;
				}
			}

			if (m_UseLocking)
			{
				m_DHAMutex.lock();
			}
			size_t realSize = CAllocationHeader::calculateNeededAllocationSize(size, align);

			/// align size to tlsf block requirements
			if (realSize > 32)
			{
				size_t tlsfalign = (1 << Foundation::highestBit(realSize >> 5)) - 1;

				if (doesAdditionOverflow(realSize, tlsfalign))
				{
					FatalErrorMsg("Size overflow in allocator.");
					return nullptr;
				}

				realSize = (realSize + tlsfalign) & ~tlsfalign;
			}

			char* newRealPtr = nullptr;
			if (size < m_RequestedPoolSize && !getPoolList(realSize).empty())
			{
				newRealPtr = (char*)tlsf_memalign(getActivePool(realSize).tlsfPool, align, realSize);
			}
			SLargeAllocations* largeAlloc = nullptr;
			if (nullptr == newRealPtr)
			{
				// only try to make new tlsfBlocks if the amount is less than a 16th of the blocksize - else spill to LargeAllocations
				if (size < m_RequestedPoolSize / 4)
				{
					// not enough space in the current block.
					// Iterate from the back, and find one that fits the allocation
					// put the found block at the head of the list
					PoolList& poolList = getPoolList(realSize);
#if 1
					for( auto pool : poolList )
					{
						newRealPtr = (char*)tlsf_memalign(pool.tlsfPool, align, realSize);
						if (newRealPtr != nullptr)
						{
							// push_front removes the node from the list and reinserts it at the start
							CMutex::CAutoLock m(m_DHAMutex);
							//poolList.push_front(*pool);
							break;
						}
					}
#else
					std::list<SPoolElement>::iterator pool = poolList.end();
					--pool;
					while (pool != poolList.end()) // List wraps around so end is the element just before the head of the list
					{
						newRealPtr = (char*)tlsf_memalign(pool->tlsfPool, align, realSize);
						if (newRealPtr != nullptr)
						{
							// push_front removes the node from the list and reinserts it at the start
							CMutex::CAutoLock m(m_DHAMutex);
							poolList.push_front(*pool);
							break;
						}
						--pool;
					}
#endif

					if (nullptr == newRealPtr)
					{
						size_t allocatePoolSize = m_RequestedPoolSize;
						void* memoryBlock = nullptr;

#if WL_ANDROID
						// @TODO:
						// On android we reload libunity (and keep activity around)
						// this results in leaks, as MemoryManager::m_InitialFallbackAllocator is never freed actually
						// more to it: even if we free the mem - the hole left is taken by other mallocs
						// so we cant reuse it on re-init, effectively allocing anew
						// this workaround can be removed when unity can be cleanly reloaded (sweet dreams)
						static const size_t _InitialFallbackAllocMemBlock_Size = 1024 * 1024;
						static bool _InitialFallbackAllocMemBlock_Taken = false;
						static char ALIGN_TYPE(16) _InitialFallbackAllocMemBlock[_InitialFallbackAllocMemBlock_Size];

						if (!_InitialFallbackAllocMemBlock_Taken)
						{
							Assert(_InitialFallbackAllocMemBlock_Size == m_RequestedPoolSize);
							_InitialFallbackAllocMemBlock_Taken = true;
							memoryBlock = _InitialFallbackAllocMemBlock;
						}
#endif

						while (!memoryBlock && allocatePoolSize > size * 2)
						{
							memoryBlock = LLAllocator::malloc(allocatePoolSize);
							if (nullptr == memoryBlock)
							{
								allocatePoolSize /= 2;
							}
							else
							{
								m_TotalReservedBytes += allocatePoolSize;
								SPoolElement* newPoolPtr = (SPoolElement*)LLAllocator::malloc(sizeof(SPoolElement));
								SPoolElement& newPool = *(new (newPoolPtr)SPoolElement());
								newPool.memoryBase = (char*)memoryBlock;
								newPool.memorySize = allocatePoolSize;
								newPool.tlsfPool = tlsf_create(memoryBlock, allocatePoolSize);
								newPool.allocationCount = 0;

								{
									CMutex::CAutoLock lock(m_DHAMutex);
									poolList.push_front(newPool);
								}

								newRealPtr = (char*)tlsf_memalign(getActivePool(realSize).tlsfPool, align, realSize);
								break;
							}
						}
					}
				}

				// System might be a bit slow when allocating large portions of memory
				int largeAllocRetries = 0;
				while (nullptr == newRealPtr)
				{
					// Large alloc uses AllocationHeaderWithSize header
					realSize = CAllocationHeaderWithSize::calculateNeededAllocationSize(size, align);
					char* largeAllocPtr = (char*)LARGE_BLOCK_ALLOC(realSize);
					if (nullptr == largeAllocPtr)
					{
						// Let system a chance to free some memory for us before quit with fatal error.
						if (++largeAllocRetries < 5)
						{
							printf_console("DynamicHeapAllocator allocation probe %d failed - Could not get memory for large allocation %llu.\n", largeAllocRetries, (unsigned long long)size);
							CThread::sleep(0.05 * largeAllocRetries);
							continue;
						}

						printf_console("DynamicHeapAllocator out of memory - Could not get memory for large allocation %llu!\n", (unsigned long long)size);
						if (m_UseLocking)
						{
							m_DHAMutex.unlock();
						}
						return nullptr;
					}

					// large allocation that don't fit on a clean block
					largeAlloc = (SLargeAllocations*)LLAllocator::malloc(sizeof(SLargeAllocations));
					largeAlloc->allocatedPtr = largeAllocPtr;

					largeAlloc->allocatedSize = realSize;
					largeAlloc->returnedSize = size;
					m_TotalReservedBytes += size;
					{
						CMutex::CAutoLock lock(m_DHAMutex);
						largeAlloc->next = m_FirstLargeAllocation;
						m_FirstLargeAllocation = largeAlloc;
					}
					newRealPtr = largeAlloc->allocatedPtr;
				}
			}

			void* ptr = nullptr;
			if (nullptr == largeAlloc)
			{
				getActivePool(realSize).allocationCount++;
				CAllocationHeader* newAllocHeader = CAllocationHeader::init(newRealPtr, m_AllocatorIdentifier, size, align);
				ptr = newAllocHeader->getUserPtr();

				registerAllocationData(getTlsfAllocationSize(newAllocHeader), newAllocHeader->getOverheadSize());
			}
			else
			{
				CAllocationHeaderWithSize* newAllocHeader = CAllocationHeaderWithSize::init(newRealPtr, m_AllocatorIdentifier, size, align);
				ptr = largeAlloc->returnedPtr = newAllocHeader->getUserPtr();

				registerAllocationData(size, largeAlloc->allocatedSize - size);
			}

			if (m_UseLocking)
			{
				m_DHAMutex.unlock();
			}
			return ptr;
		}
		/// Reallocate memory block at the specified pointer.
		virtual void* reallocate(void* p, size_t size, int align) final
		{
			void* ptr = nullptr;
			return ptr;
		}

		/// Deallocate memory block.
		virtual void   deallocate(void* p) final
		{

		}
		private:
			mutable CMutex m_DHAMutex;
			bool m_UseLocking = false;
			/// Lockless fixed-size allocator for small memory blocks.
			CBucketAllocator* m_BucketAllocator = nullptr;
			size_t m_RequestedPoolSize = 0;
			size_t m_SplitLimit = 0; 
			PoolList m_SmallTLSFPools;
			PoolList m_LargeTLSFPools;

	};
}