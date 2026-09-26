#include "GraphicPlug.h"
#include "MLEngine.h"
#include "DX12Device.h"

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
			return mpDeviceBase->initDevice(GEngine->GetPlatform().getMainWnd());
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