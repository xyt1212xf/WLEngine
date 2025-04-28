#pragma once
#include "Common.h"
#include "MemoryMgr.h"
#include "Export.h"

namespace WL
{
	class WLENGINE_API CRefcount
	{
	public:
		CRefcount();
		virtual ~CRefcount();
		virtual	void addRef();
		virtual void release();
		unsigned short getRefCount();

	protected:
		unsigned short mCount;
	};
}
