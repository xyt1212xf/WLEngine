#include "GpuBufferBase.h"
namespace WL
{

	CGpuBufferBase::CGpuBufferBase()
	{

	}

	CGpuBufferBase::~CGpuBufferBase()
	{

	}

	void CGpuBufferBase::setType(int nType)
	{
		mMemoryType = nType;
	}

	int CGpuBufferBase::getType() const
	{
		return mMemoryType;
	}

}