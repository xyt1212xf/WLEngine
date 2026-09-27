#include "TimerClock.h"
namespace ML
{
	void CTimerClock::begin()
	{
		mStart = std::chrono::high_resolution_clock::now();
	}

	void CTimerClock::reset()
	{
		begin();
	}

	double CTimerClock::getTimerSecond()
	{
		return getTimerMicroSec() * 0.000001;
	}

	double CTimerClock::getTimerMilliSec()
	{
		return getTimerMicroSec() * 0.001;
	}

	INT64 CTimerClock::getTimerMicroSec()
	{
		return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - mStart).count();
	}

}

