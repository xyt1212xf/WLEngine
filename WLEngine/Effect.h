#pragma once
#include "Entity.h"
#ifdef D3D11
#include "DX11Device.h"
#endif
namespace WL
{
	class CEffect : public CEntity
	{
	public:
		CEffect();
		virtual ~CEffect();
		virtual void setEffect(DeviceContext* pContext);
	};
}