#pragma once
#include "BaseAllocator.h"
#include "DelayedPointerDeletionManager.h"
#include "BucketAllocator.h"

namespace WL
{ 
	class CBucketAllocator;

	template <class UnderlyingAllocator>
	class CDualThreadAllocator : public CBaseAllocator
	{
	public:
		// when constructing it will be from the main thread
		CDualThreadAllocator(const char* name, CBucketAllocator* bucketAllocator, CBaseAllocator* mainAllocator, CBaseAllocator* threadAllocator)
		: CBaseAllocator(name)
		, m_BucketAllocator(bucketAllocator)
		, m_MainAllocator((UnderlyingAllocator*)mainAllocator)
		, m_ThreadAllocator((UnderlyingAllocator*)threadAllocator)
		{

		}
		virtual ~CDualThreadAllocator() 
		{

		}

		virtual void* allocate(size_t size, int align) final
		{
			// Try lockless allocator first
			if (m_BucketAllocator != nullptr && m_BucketAllocator->CBucketAllocator::canAllocate(size, align))
			{
				void* ptr = m_BucketAllocator->CBucketAllocator::allocate(size, align);
				if (ptr != nullptr)
				{
					return ptr;
				}
			}

			// Use either main thread allocator or thread allocator
			UnderlyingAllocator* alloc = getCurrentAllocator();
			bool isMainThread = alloc == m_MainAllocator;
			if (isMainThread && m_DelayedDeletion && m_DelayedDeletion->hasPending())
			{
				m_DelayedDeletion->cleanupPendingMainThreadPointers();
			}
			return alloc->UnderlyingAllocator::allocate(size, align);
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

		UnderlyingAllocator* getCurrentAllocator() const
		{
			if (CThread::CurrentThreadIsMainThread())
			{
				return m_MainAllocator;
			}
			else
			{
				return m_ThreadAllocator;
			}
		}

	private:
		CBucketAllocator* m_BucketAllocator;
		UnderlyingAllocator* m_MainAllocator = nullptr;
		UnderlyingAllocator* m_ThreadAllocator = nullptr;
		CDelayedPointerDeletionManager* m_DelayedDeletion = nullptr;
	};
}