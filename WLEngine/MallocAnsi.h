#pragma once
#include "MemoryBase.h"
namespace WL
{
	void* AnsiMalloc(SIZE_T Size, uint32 Alignment);
	void* AnsiRealloc(void* Ptr, SIZE_T NewSize, uint32 Alignment);
	void AnsiFree(void* Ptr);
	class FMallocAnsi final : public FMalloc  
	{
	public:
		/**
		 * Constructor enabling low fragmentation heap on platforms supporting it.
		 */
		FMallocAnsi();

		virtual void* Malloc(SIZE_T Count, uint32 Alignment = DEFAULT_ALIGNMENT) override;
		virtual void* TryMalloc(SIZE_T Size, uint32 Alignment) override;
		virtual void* Realloc(void* Ptr, SIZE_T NewSize, uint32 Alignment) override;
		virtual void* TryRealloc(void* Ptr, SIZE_T NewSize, uint32 Alignment) override;
		virtual void Free(void* Ptr) override;
	};
}