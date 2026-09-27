#include "GraphicPlug.h"
#include "MLEngine.h"
#include "DX12Device.h"
#include "Renderer.h"
#include "TimerClock.h"

namespace ML
{

	CGraphicPlug::CGraphicPlug()
	{
		
	}

	CGraphicPlug::~CGraphicPlug()
	{
		if (mpDeviceBase)
		{

		}
	}

	bool CGraphicPlug::Initialise()
	{
		if (nullptr == mpDeviceBase)
		{
			mpDeviceBase = new CDX12Device;
			if (mpDeviceBase->initDevice(GEngine->GetPlatform().getMainWnd()))
			{
				GEngine->threadDetach([this]()
					{
						CThreadPool::setThreadName("RenderThread", GetCurrentThreadId());
						mpRenderer->_Draw();
					});
				return true;
			}
		}	
		return false;
	}

	bool CGraphicPlug::UnInitialise()
	{
		return false;
	}

	void CGraphicPlug::Process(int32 DeltaSeconds) const
	{

	}

}