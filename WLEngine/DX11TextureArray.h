#pragma once
#include "Resource.h"
#include "Texture.h"

namespace WL
{
	class CDX11TextureArray : public CResource
	{
	public:
		CDX11TextureArray() = default;
		virtual ~CDX11TextureArray();
		bool addTexture(const std::string& szTexture, bool bGenerateMips = false);
		void addTexture(CTexture* pTexture);
		void removeTexture(CTexture* pTexture);
		void removeTexture(size_t nIndex);
		void clearAllTextures();
		void operator =(const CDX11TextureArray& textureaArray);
		void setSamplerState(SamplerState* pSampleState);

	protected:
		int mTextureCount = 0;
		ID3D11Texture2D* mpTexture = nullptr;
		ID3D11ShaderResourceView* mShaderRV = nullptr;
		ID3D11SamplerState* mpTextureSamplerState = nullptr;
		std::vector<CDX11Texture*> mTextureVector;
		std::vector<ID3D11ShaderResourceView*> mShaderRVVector;
		std::vector<ID3D11SamplerState*> mSampleStateVector;
	};
}