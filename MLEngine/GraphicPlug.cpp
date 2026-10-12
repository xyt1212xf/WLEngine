#include "GraphicPlug.h"
#include "MLEngine.h"
#include "Device.h"
#include "Renderer.h"
#include "TimerClock.h"
#include "DX12RHIbuffer.h"
#include "GlobalShader.h"

namespace ML
{

	CGraphicPlug::CGraphicPlug()
	{
#ifdef _DEBUG
		std::cout<<this;
#endif
	}

	CGraphicPlug::~CGraphicPlug()
	{
		SafeDelete(mpDeviceBase);
	}


	bool CGraphicPlug::Initialise()
	{
		if (nullptr == mpDeviceBase)
		{
			mpDeviceBase = new CRHIDevice;
			if (mpDeviceBase->initDevice(GEngine->GetPlatform().getMainWnd()))
			{
				// Compile all registered global shaders once the device is ready.
				CGlobalShaderMap::InitGlobalShaders();
				mpRenderer = new CRenderer(this);
				return true;
			}
		}	
		return false;
	}

	bool CGraphicPlug::UnInitialise()
	{
		CGlobalShaderMap::DestroyGlobalShaders();
		SafeDelete(mpRenderer);
		SafeDelete(mpDeviceBase);
		return true;
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

	CRHIDevice* CGraphicPlug::GetDevice()
	{
		return mpDeviceBase;
	}

}