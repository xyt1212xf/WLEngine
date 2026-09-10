#include "TimerEngine.h"
#include "WLEngine.h"

namespace WL
{
	const static int timeCell = 10;

	CTimerEngine::CTimerEngine()
	{
		mCallBackList.resize(256);
		mCurrentTime = GetTickCount();
	}

	CTimerEngine::~CTimerEngine()
	{
		for (auto& item : mCallBackList)
		{
			for (auto& _item : item)
			{
				SafeDelete(_item);
			}
		}
	}

	bool CTimerEngine::initialize()
	{
		GEngine->threadDetach([]()
			{
				GEngine->onRun();
			});
		return true;
	}

	void CTimerEngine::unInitialize()
	{
		mbOver = true;
		while (!mbThredOver)
		{
			Sleep(5);
		}
	}

	void CTimerEngine::registerCall(CCallBack* pCallBack, int runCount /*= 0*/, int DealyRunTime /*= 0 */)
	{
		static int loopTime = timeCell * 256;
		pCallBack->mLoop = pCallBack->mAlternationTime / loopTime;
		int nSocket = (pCallBack->mAlternationTime - pCallBack->mLoop *loopTime ) / timeCell;
		BYTE nScoketIndex = mCurrentIndex + nSocket;
		mCallBackList[nScoketIndex].push_back(pCallBack);
	}

	void CTimerEngine::onRun()
	{
		while (!mbOver)
		{
			DWORD time = GetTickCount();
			if (time - mCurrentTime < timeCell)
			{
				continue;
			}
			auto& callbackList = mCallBackList[mCurrentIndex];
			auto item = callbackList.begin();
			auto end = callbackList.end();
			while (item != end)
			{
				if (0 != (*item)->mLoop)
				{
					--(*item)->mLoop;
					++item;
					continue;
				}
				(*item)->onRun((*item)->mCopyParameters);
				if ((*item)->mRunCount > 0)
				{
					if (0 != --(*item)->mRunCount)
					{
						registerCall((*item), (*item)->mRunCount, 0);
					}
					else
					{
						SafeDelete((*item));
					}
				}
				else
				{
					registerCall((*item), (*item)->mRunCount, 0);
				}
				item = callbackList.erase(item);
				
			}

			DWORD nowTime = GetTickCount();
			DWORD sleepTime = nowTime - mCurrentTime;
			if (sleepTime < timeCell)
			{
				Sleep(timeCell - sleepTime);
			}
			mCurrentTime += timeCell;
			++mCurrentIndex;
		}
		mbThredOver = true;
	}
}