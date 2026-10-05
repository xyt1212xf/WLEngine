#include "GraphicPlug.h"
#include "MLEngine.h"
#include "DX12Device.h"
#include "Renderer.h"
#include "TimerClock.h"
#include "DX12RHIbuffer.h"

namespace ML
{

	CGraphicPlug::CGraphicPlug()
	{
		std::cout<<this;
	}

	CGraphicPlug::~CGraphicPlug()
	{
		SafeDelete(mpDeviceBase);
	}


	bool CGraphicPlug::Initialise()
	{
		if (nullptr == mpDeviceBase)
		{
			mpDeviceBase = new CDX12RHIDevice;
			if (mpDeviceBase->initDevice(GEngine->GetPlatform().getMainWnd()))
			{
				mpRenderer = new CRenderer(this);
				return true;
			}
		}	
		return false;
	}

	bool CGraphicPlug::UnInitialise()
	{
		return false;
	}

	void CGraphicPlug::Process(float DeltaSeconds) const
	{

	}

	void CGraphicPlug::Start()
	{
		GEngine->threadDetach([&]()
			{
				CThreadPool::setThreadName("RenderThread", GetCurrentThreadId());
				mpRenderer->_Draw();
			});
	}

	CDX12RHIDevice* CGraphicPlug::GetDevice()
	{
		return mpDeviceBase;
	}

}