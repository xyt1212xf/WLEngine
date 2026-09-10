#pragma once
#include "RenderStateHeader.h"
#include "Device.h"
#include "RenderState.h"
namespace WL
{
	class CRenderStateMgrBase	
	{
	public:
		CRenderStateMgrBase(CDevice* pDevice);
		virtual ~CRenderStateMgrBase();
		virtual SRenderState* createRenderState(SRenderStateDesc& renderStateDesc);
	protected:
		CDevice* mpDevice = nullptr;
	};
}