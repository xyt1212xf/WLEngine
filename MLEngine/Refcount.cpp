#include "Refcount.h"
namespace ML
{
	CRefcount::CRefcount()
	: mCount(1)
	{

	}

	CRefcount::~CRefcount()
	{

	}

	void CRefcount::addRef()
	{
		++mCount;
	}

	void CRefcount::release()
	{
		if (--mCount == 0)
		{
			delete this;
		}
	}

	unsigned short CRefcount::getRefCount()
	{
		return mCount;
	}
}

