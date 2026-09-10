#include "SurfaceView.h"
namespace WL
{
	CSurfaceView::CSurfaceView(RenderTargetView* pRTTView, DepthStencilView* pDepthStencil)
#ifdef D3D11
	: CDX11SurfaceView(pRTTView, pDepthStencil)
#endif
	{

	}

	CSurfaceView::~CSurfaceView()
	{

	}

}