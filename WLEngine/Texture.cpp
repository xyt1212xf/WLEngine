#include "Texture.h"
namespace WL
{

	CTexture::CTexture()
	{

	}

	CTexture::~CTexture()
	{

	}

	void CTexture::use(DeviceContext* pContext, int nSlot)
	{
		if (mpSampleState != nullptr)
		{
			pContext->PSSetSamplers(nSlot, 1, &mpSampleState);
		}
		if (mpShaderResourceView != nullptr)
		{
			pContext->PSSetShaderResources(nSlot, 1, &mpShaderResourceView);
		}
	}

}