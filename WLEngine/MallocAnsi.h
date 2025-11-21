#pragma once
#include "MemoryBase.h"
namespace WL
{
	class FMallocAnsi final : public FMalloc  
	{
	public:
		/**
		 * Constructor enabling low fragmentation heap on platforms supporting it.
		 */
		FMallocAnsi();
	};
}