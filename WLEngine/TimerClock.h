#pragma once
#include <chrono>
#include "Common.h"
namespace WL
{
	class CTimerClock
	{
	public:
		CTimerClock() = default;

		~CTimerClock() = default;

		void begin();

		void reset();
		//获取秒
		double getTimerSecond();

		//获取毫秒
		double getTimerMilliSec();
	
		//获取微妙
		INT64 getTimerMicroSec();

	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> mStart;
	};

}