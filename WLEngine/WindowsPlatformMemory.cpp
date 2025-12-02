#include "WindowsPlatformMemory.h"
namespace WL
{
	LPVOID __stdcall MiMallocVirtualAlloc(LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect)
	{
		LPVOID Result = VirtualAlloc(lpAddress, dwSize, flAllocationType, flProtect);
		//if (Result && FLowLevelMemTracker::IsEnabled())
		//{
		//	if (flAllocationType & MEM_RESERVE)
		//	{
		//		SIZE_T dwOldSize;
		//		GetVirtualAllocPageStatus().AddReservationSize(Result, dwSize, dwOldSize);
		//		LLMCheck(dwOldSize == 0 || dwSize == dwOldSize);
		//	}
		//	if (flAllocationType & MEM_COMMIT)
		//	{
		//		int64 DeltaSize = GetVirtualAllocPageStatus().MarkChangedAndReturnDeltaSize(Result, dwSize, true /* bCommitted */);
		//		LLM_PLATFORM_SCOPE(ELLMTag::FMalloc);
		//		FLowLevelMemTracker::Get().OnLowLevelChangeInMemoryUse(ELLMTracker::Platform, DeltaSize);
		//	}
		//}
		return Result;
	}

	void FWindowsPlatformMemory::MiMallocInit()
	{

	}

}