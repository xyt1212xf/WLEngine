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

	RenderState* CDeviceBase::createRenderState(RenderStateDesc& renderStateDesc)
	{
		return nullptr;
	}

}