#include "DX11RenderState.h"
namespace WL
{

	DX11RenderState::~DX11RenderState()
	{

		pDepthStencilState = nullptr;
		pBlendState = nullptr;
		pRasterizerState = nullptr;

	}

}