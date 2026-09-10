#pragma once
#include "Resource.h"
#include "GraphicHead.h"

namespace WL
{
	class CGpuBufferBase : public CResource
	{
	public:
		CGpuBufferBase();
		virtual ~CGpuBufferBase();
		void setType(int nType);
		int getType()const;

	protected:
		int mMemoryType = 0;
	};
}