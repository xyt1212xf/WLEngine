#include "ShadowEffect.h"
#include "WLEngine.h"
#include "SurfaceView.h"
namespace WL
{
	CShadowEffect::CShadowEffect()
	{
	}

	CShadowEffect::~CShadowEffect()
	{
		WL_DECREASE(mpSurfaceView);
	}

	bool CShadowEffect::buildSurfaceView(int nWidth, int nHeight, UINT32 depthFormat, UINT bindInfo, UINT32 nTextureFormat, UINT32 miscFlags)
	{
		mpSurfaceView = GEngine->createRendererSurface(nWidth, nHeight, depthFormat, bindInfo, nTextureFormat, miscFlags);
		if (nullptr != mpSurfaceView)
		{
			mpSurfaceView->addRef();
			return true;
		}
		else
		{
			return false;
		}
	}

	void CShadowEffect::buildSurfaceView(CSurfaceView* pRenderSurface)
	{
		if (mpSurfaceView != pRenderSurface)
		{
			WL_DECREASE(mpSurfaceView);
		}
		mpSurfaceView = pRenderSurface;
		if (nullptr != mpSurfaceView)
		{
			mpSurfaceView->addRef();
		}
	}

	void CShadowEffect::setEffect(DeviceContext* pContext)
	{
		
	}

}