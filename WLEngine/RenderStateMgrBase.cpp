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

	SRenderState* CRenderStateMgrBase::createRenderState(SRenderStateDesc& renderStateDesc)
	{
		return nullptr;
	}

}