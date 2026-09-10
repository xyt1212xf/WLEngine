#pragma once

#ifdef D3D11
#include "DX11RenderStateMgr.h"
#endif
namespace WL
{
	class CDevice;
#ifdef D3D11
	class CRenderStateMgr : public  CDX11RenderStateMgr
#endif
	{
	public:
		CRenderStateMgr(CDevice* pDevice);
		virtual ~CRenderStateMgr();
		//bool setRenderState(StateFlag flag, RenderState* pRS);
	};
}