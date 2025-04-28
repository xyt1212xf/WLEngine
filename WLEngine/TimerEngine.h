#pragma once
#include "CallBack.h"
#include "EngineBase.h"

namespace WL
{
	class CTimerEngine :virtual public CEngineBase
	{
	public:
		CTimerEngine();
		virtual ~CTimerEngine();
		bool initialize();
		void unInitialize();
		void registerCall(CCallBack* pCallBack, int runCount = 0, int DealyRunTime = 0 );
		void onRun();

	private:
		std::atomic_bool 	mbOver = false;
		std::vector<std::list<CCallBack*> > mCallBackList;
		bool 	mbThredOver = false;
		DWORD mCurrentTime = 0;
		BYTE mCurrentIndex = 0;
	};
}