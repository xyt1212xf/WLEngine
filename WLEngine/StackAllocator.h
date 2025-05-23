#pragma once
#include "BaseAllocator.h"
namespace WL
{
	class CStackAllocator : public CBaseAllocator
	{
	public:
		CStackAllocator(size_t blocksize, SMemLabelId fallbackMemLabel, const char* name);
		virtual ~CStackAllocator();
		/// Allocate memory block of the specified size (minimum size) and alignment.
		virtual void* allocate(size_t size, int align) final;
		/// Reallocate memory block at the specified pointer.
		virtual void* reallocate(void* p, size_t size, int align) final;
		/// Deallocate memory block.
		virtual void   deallocate(void* p) final;
	};
}