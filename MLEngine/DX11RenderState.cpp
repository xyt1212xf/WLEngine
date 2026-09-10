#include "DX11RenderState.h"
namespace WL
{

	SDX11RenderState::~SDX11RenderState()
	{

		pDepthStencilState = nullptr;
		pBlendState = nullptr;
		pRasterizerState = nullptr;

	}

}