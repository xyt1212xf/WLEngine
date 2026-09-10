#pragma once
#include "Common.h"
namespace WL
{
	// AtomicAdd - Returns the new value, after the operation has been performed (as defined by OSAtomicAdd32Barrier)
	INLINE int atomicAdd(int volatile* i, int value)
	{
		return _InterlockedExchangeAdd((long volatile*)i, value) + value;
	}

	// AtomicAdd for size_t
	INLINE size_t atomicAdd(size_t volatile* i, size_t value)
	{
		//CompileTimeAssert(sizeof(size_t) == sizeof(int), "size_t must be 4byte in !UNITY_64");
		return (size_t)atomicAdd((int volatile*)i, (int)value);
	}

	// AtomicExchange - Returns the initial value pointed to by Target (as defined by _InterlockedExchange)
	INLINE int atomicExchange(int volatile* i, int value)
	{
		return (int)_InterlockedExchange((long volatile*)i, (long)value);
	}

	INLINE bool atomicCompareExchange(int volatile* i, int newValue, int expectedValue)
	{
		return _InterlockedCompareExchange((long volatile*)i, (long)newValue, (long)expectedValue) == expectedValue;
	}

	// AtomicSub for size_t
	INLINE bool atomicCompareExchange(size_t volatile* i, size_t newValue, size_t expectedValue)
	{
		//CompileTimeAssert(sizeof(size_t) == sizeof(int), "size_t must be 4byte in !UNITY_64");
		return atomicCompareExchange((int volatile*)i, (int)newValue, (int)expectedValue);
	}


	// AtomicIncrement - Returns the new value, after the operation has been performed (as defined by OSAtomicAdd32Barrier)
	INLINE int atomicIncrement(int volatile* i)
	{
#ifdef _WINDOWS
		return _InterlockedIncrement((long volatile*)i);
#else
		return atomicAdd(i, 1);
#endif
	}

	// AtomicDecrement - Returns the new value, after the operation has been performed (as defined by OSAtomicAdd32Barrier)
	INLINE int atomicDecrement(int volatile* i)
	{
#ifdef _WINDOWS
		return _InterlockedDecrement((long volatile*)i);
#else
		return AtomicSub(i, 1);
#endif
	}
#ifdef  WIN32
	typedef INT32 atomic_word;
	union atomic_word2
	{
#	if defined(_MSC_VER)
		__int64 v;
#	else
		long long v;
#	endif
#	if !defined(__SSE2__)
		double d;
#	endif
		struct
		{
			atomic_word lo, hi;
		};
	};
#endif
}