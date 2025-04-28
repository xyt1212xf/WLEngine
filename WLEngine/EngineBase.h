#pragma once
#include "Device.h"
namespace WL
{
	class CEngineBase
	{
	public:
		CEngineBase()
		{
			mCommandBufIndex = 0;
		}
		virtual void _update(UINT32 ) {}
		void commitCommand(commandType nType, void* pData = nullptr)
		{
			lockCommandBuf();
			mCommandBuf.emplace_back(command(nType, pData));
			unLockCommandBuf();
		}
		void lockCommandBuf()
		{
			mCommandBufMutex.lock();
		}
		void unLockCommandBuf()
		{
			mCommandBufMutex.unlock();
		}
	protected:
		std::atomic<UINT32> mCommandBufIndex;
		std::mutex	mCommandBufMutex;
		std::list<command>	mCommandBuf;	
	};



}