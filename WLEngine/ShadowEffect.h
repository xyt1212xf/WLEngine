#pragma once
#include "Effect.h"
#include "SurfaceView.h"
namespace WL
{
	class CShadowEffect : public CEffect
	{
		friend class CShadowRenderer;
	public:
		CShadowEffect();
		virtual ~CShadowEffect();
		bool buildSurfaceView(int nWidth, int nHeight, UINT32 depthFormat, UINT bindInfo, UINT32 nTextureFormat, UINT32 miscFlags);
		void buildSurfaceView(CSurfaceView* pRenderSurface);
		virtual void setEffect(DeviceContext* pContext) final;
		
		
	protected:
		CSurfaceView* mpSurfaceView = nullptr;
	};
}