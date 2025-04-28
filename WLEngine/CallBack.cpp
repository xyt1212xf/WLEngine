#include "CallBack.h"

namespace WL 
{
	CCallBack::CCallBack()
		:CCallBack(nullptr)
	{
	}

	CCallBack::CCallBack(void* pParameters)
	{
		mCopyParameters = pParameters;
		mTimerID = -1;
		mRunCount = 0;
		mDealyRunTime = 0;
		mAlternationTime = 20;
	}

	CCallBack::~CCallBack()
	{

	}

	void CCallBack::onRun(void* pParameters)
	{

	}

}

