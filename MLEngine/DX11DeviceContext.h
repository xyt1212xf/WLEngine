#pragma once

#include "DX11Device.h"
namespace WL
{
	class CDX11DeviceContext
	{
	public:
		CDX11DeviceContext(ID3D11DeviceContext* pContext);
		virtual ~CDX11DeviceContext();
		ID3D11DeviceContext* operator->();
		ID3D11DeviceContext* operator()() const;

	private:
		ID3D11DeviceContext* mpContext = nullptr;
		ID3D11RasterizerState* mpCurrentRasterState = nullptr;
		ID3D11BlendState* mpCurrentBlendingState = nullptr;
		ID3D11DepthStencilState* mpCurrentDepthStencilState = nullptr;
	};
}