#pragma once
#ifdef D3D11
#include "DX11SurfaceView.h"
#endif

namespace WL
{
#ifdef D3D11
	class CSurfaceView : public CDX11SurfaceView 
#endif
	{
	public:
		CSurfaceView() = default;
		CSurfaceView(RenderTargetView* pRTTView, DepthStencilView* pDepthStencil);
		virtual ~CSurfaceView();
	};
}