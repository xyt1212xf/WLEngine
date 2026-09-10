#include "ThreadsafeLinearAllocator.h"
namespace WL
{

	CThreadsafeLinearAllocator::CThreadsafeLinearAllocator(int blockSize, int maxBlocksCount, const char* name)
	: CBaseAllocator(name)
	{

	}

	CThreadsafeLinearAllocator::~CThreadsafeLinearAllocator()
	{

	}
	void* CThreadsafeLinearAllocator::allocate(size_t size, int align)
	{
		void* ptr = nullptr;
		return ptr;
	}

	void* CThreadsafeLinearAllocator::reallocate(void* p, size_t size, int align)
	{
		void* ptr = nullptr;
		return ptr;
	}

	void CThreadsafeLinearAllocator::deallocate(void* p)
	{

	}
}