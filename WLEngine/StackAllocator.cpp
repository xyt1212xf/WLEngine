#include "StackAllocator.h"
namespace WL
{
	CStackAllocator::CStackAllocator(size_t blocksize, MemLabelId fallbackMemLabel, const char* name)
	: CBaseAllocator(name)
	{

	}

	CStackAllocator::~CStackAllocator()
	{

	}

	void* CStackAllocator::allocate(size_t size, int align)
	{
		void* ptr = nullptr;
		return ptr;
	}

	void* CStackAllocator::reallocate(void* p, size_t size, int align)
	{
		void* ptr = nullptr;
		return ptr;
	}

	void CStackAllocator::deallocate(void* p)
	{

	}

}