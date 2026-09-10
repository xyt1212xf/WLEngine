#include "AllocationHeader.h"
namespace WL
{

	void CAllocationSizeHeader::initAllocationSizeHeader(void* allocPtr, size_t padCount, size_t size)
	{
		// We assume, that allocPtr MUST be at least 4 bytes aligned to fit possible padding length
		DebugAssert(allocPtr == alignPtr(allocPtr, 4));
		if (padCount > 0) // set leading bytes
		{
			memset(allocPtr, kPadValue, padCount);
		}
		setPaddingCount(padCount);
		m_AllocationSize = size;
	}

	bool CAllocationSizeHeader::hasPadding() const
	{
		return m_HasPadding; 
	}

	size_t CAllocationSizeHeader::getAllocationSize() const
	{
		return m_AllocationSize;
	}


	void CAllocationSizeHeader::setPaddingCount(size_t padding)
	{
		m_HasPadding = padding > 0;
		if (padding > 0)
		{
			*((UINT32*)this - 1) = padding;
		}
	}

	size_t CAllocationSizeHeader::getPaddingCount() const
	{
		return m_HasPadding ? *((const UINT32*)this - 1) : 0;
	}

}