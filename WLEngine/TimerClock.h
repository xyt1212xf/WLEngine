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
		//��ȡ��
		double getTimerSecond();

		//��ȡ����
		double getTimerMilliSec();
	
		//��ȡ΢��
		INT64 getTimerMicroSec();

	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> mStart;
	};

}