#pragma once
#include "Common.h"

namespace ML
{
	class CRefcount
	{
	public:
		CRefcount();
		virtual ~CRefcount();
		virtual	void addRef();
		virtual void release();
		unsigned short getRefCount();

	protected:
		unsigned short mCount = 0;
	};
}
