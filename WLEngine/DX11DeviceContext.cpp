#include "DX11DeviceContext.h"
namespace WL
{
	CDX11DeviceContext::CDX11DeviceContext(ID3D11DeviceContext* pContext)
	: mpContext(pContext)
	{

	}

	CDX11DeviceContext::~CDX11DeviceContext()
	{
		DEV_RELEASE(mpContext);
		mpCurrentRasterState = nullptr;
		mpCurrentBlendingState = nullptr;
		mpCurrentDepthStencilState = nullptr;
	}

	ID3D11DeviceContext* CDX11DeviceContext::operator->()
	{
		return mpContext;
	}

	ID3D11DeviceContext* CDX11DeviceContext::operator()() const
	{
		return mpContext;
	}

}