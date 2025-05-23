#pragma once
#include "DX11Device.h"
namespace WL
{
#define  DepthStencilState ID3D11DepthStencilState 
#define  BlendState ID3D11BlendState  
#define  RasterizerState ID3D11RasterizerState
	struct SDX11RenderState
	{
		DepthStencilState* pDepthStencilState = nullptr;
		BlendState* pBlendState = nullptr;
		RasterizerState* pRasterizerState = nullptr;
		virtual ~SDX11RenderState();
	};
}