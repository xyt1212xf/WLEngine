#pragma once
#include "Device.h"
#ifdef D3D11
#include "DX11Texture.h"
#endif

namespace WL
{
#ifdef D3D11
	class CTexture : public CDX11Texture
#endif
	{
		friend class CSurfaceView;
	public:
		CTexture();
		virtual ~CTexture();
		void use(DeviceContext* pContext, int nSlot);
	};

}

