#include "RenderStateMgrBase.h"
namespace WL
{
	CRenderStateMgrBase::CRenderStateMgrBase(CDevice* pDevice)
	: mpDevice(pDevice)
	{

	}

	CRenderStateMgrBase::~CRenderStateMgrBase()
	{

	}

	RenderState* CRenderStateMgrBase::createRenderState(RenderStateDesc& renderStateDesc)
	{
		return nullptr;
	}

}