#include "DeviceBase.h"
#include "RenderStateMgr.h"
#include "RenderState.h"

namespace WL
{

	CDeviceBase::CDeviceBase()
	{

	}

	CDeviceBase::~CDeviceBase()
	{
		SafeDelete(mpRenderStateMgr);
	}

	SRenderState* CDeviceBase::createRenderState(SRenderStateDesc& renderStateDesc)
	{
		return nullptr;
	}

}