#include "DX11Material.h"
#include "WLEngine.h"

namespace WL
{

	CDX11Material::CDX11Material()
	{

	}

	CDX11Material::~CDX11Material()
	{

	}

	bool CDX11Material::initialiseSampler(SamplerDesc* pSampler)
	{
		// Load the texture in.
		CDevice* pDevice = GEngine->getDevice();

		// Create the texture sampler state.
		ID3D11SamplerState* pSampleState = nullptr;
		HRESULT result = pDevice->getGraphicsDevice()->CreateSamplerState((D3D11_SAMPLER_DESC*)pSampler, &pSampleState);
		if (FAILED(result))
		{
			return false;
		}
		mSampleState.push_back(pSampleState);
		return true;
	}
}