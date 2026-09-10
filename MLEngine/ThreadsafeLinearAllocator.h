#pragma once
#include "BaseAllocator.h"
namespace WL
{
	class CThreadsafeLinearAllocator : public CBaseAllocator
	{
	public:
		// Construct ThreadsafeLinearAllocator.
		// @param blockSize Fixed block size.
		// @param maxBlocksCount Maximum blocks count (<= 255).
		// @param blockMemLabel Memory label for allocating blocks' memory.
		// @param name Allocator name (fore debugging purposes).
		CThreadsafeLinearAllocator(int blockSize, int maxBlocksCount, const char* name);
		virtual ~CThreadsafeLinearAllocator();
		/// Allocate memory block of the specified size (minimum size) and alignment.
		virtual void* allocate(size_t size, int align) final;
		/// Reallocate memory block at the specified pointer.
		virtual void* reallocate(void* p, size_t size, int align) final;
		/// Deallocate memory block.
		virtual void   deallocate(void* p) final;
	};
}