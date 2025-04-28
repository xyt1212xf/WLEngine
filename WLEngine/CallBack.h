#pragma once
#include "Entity.h"
namespace WL 
{
	struct CCallBack : public CEntity
	{
		CCallBack();
		CCallBack(void* pParameters);
		virtual ~CCallBack();
		virtual void onRun(void* pParameters);

		void* mCopyParameters = nullptr;
		int mTimerID = -1;
		int mRunCount = 0;
		int mDealyRunTime = 0;
		int mAlternationTime = 20;
		int mLoop = 0;
	};
}