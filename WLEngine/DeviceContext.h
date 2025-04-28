#pragma once
#ifdef D3D11
#include "DX11DeviceContext.h"
#endif

namespace WL
{
#ifdef D3D11
	class CDeviceContext : public CDX11DeviceContext
#endif
	{
	public:
#ifdef D3D11
		CDeviceContext(ID3D11DeviceContext* pContext);
#endif
		virtual ~CDeviceContext();
	};

}