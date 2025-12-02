#pragma once
#include "MemoryBase.h"
#include "WindowsPlatformMemory.h"

namespace WL
{
	class FMallocMimalloc final : public FMalloc
	{
	public:
		FMallocMimalloc();
		virtual void* Malloc(SIZE_T Size, uint32 Alignment) override;
		virtual void* TryMalloc(SIZE_T Size, uint32 Alignment) override;
		virtual void* Realloc(void* Ptr, SIZE_T NewSize, uint32 Alignment) override;
		virtual void* TryRealloc(void* Ptr, SIZE_T NewSize, uint32 Alignment) override;
		virtual void Free(void* Ptr) override;

	protected:

		void OutOfMemory(uint64 Size, uint32 Alignment)
		{
			// this is expected not to return
			FPlatformMemory::OnOutOfMemory(Size, Alignment);
		}
	};
}