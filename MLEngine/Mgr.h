#pragma once
#include "Common.h"

namespace ML
{
	class MLENGINE_API CMgr
	{
	public:
		virtual ~CMgr() = default;

		virtual const char* GetName() const = 0;

		virtual bool Init() = 0;

		virtual void Shutdown() = 0;
	};
}
