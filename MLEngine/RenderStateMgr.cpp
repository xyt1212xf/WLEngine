
#include "RenderStateMgr.h"
#include "Device.h"
namespace WL
{
	CRenderStateMgr::CRenderStateMgr(CDevice* pDevice)
	: CDX11RenderStateMgr(pDevice)
	{

	}

	CRenderStateMgr::~CRenderStateMgr()
	{

	}

	//bool CRenderStateMgr::setRenderState(StateFlag flag, RenderState* pRS)
	//{
	//	switch (flag)
	//	{
	//	case DepthStencilOn:
	//	case DepthStencilOff:
	//		return setDepthStencil(pRS);
	//	case AlphaOn:
	//	case AlphaOff:
	//		return setAlpha(pRS);
	//	case ZBufferOn:
	//	case ZBufferOff:
	//		return setDepth(pRS);
	//	case FrontCull:
	//	case BackCull:
	//	case NoCull:
	//		return setCullMode(pRS);
	//	case Fill_WireFrame:
	//	case Fill_Solid:
	//		return setFillMode(pRS);
	//	default:
	//		return false;
	//	}
	//}



}