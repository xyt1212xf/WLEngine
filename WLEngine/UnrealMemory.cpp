#include "UnrealMemory.h"
namespace WL
{

	void FMemory::Free(void* Original)
	{
		if (nullptr == Original)
		{
			FScopedMallocTimer Timer(3);
			return;
		}


	}

	SIZE_T FMemory::GetAllocSize(void* Original)
	{
		SIZE_T Result = 0;
		return Result;
	}

}