#include "Refcount.h"
namespace WL
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
			delete_internal(this, MemLabelRef(kMemInstanceId));
		}
	}

	unsigned short CRefcount::getRefCount()
	{
		return mCount;
	}
}

