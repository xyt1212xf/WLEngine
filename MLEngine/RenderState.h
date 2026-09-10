#pragma once
#include "Common.h"
#ifdef D3D11 
#include "DX11RenderState.h"
#endif
namespace WL
{
#ifdef D3D11 
	struct SRenderState : public SDX11RenderState
#endif
	{
		UINT32 mId = 0;
		int nStencilRef = 0;
	};
}