#include "DX11TextureArray.h"
#include "WLEngine.h"

namespace WL
{

	CDX11TextureArray::~CDX11TextureArray()
	{
		for (auto item : mTextureVector)
		{
			WL_DECREASE(item);
		}
		mTextureVector.clear();
		mShaderRVVector.clear();
		mSampleStateVector.clear();

		DEV_RELEASE(mShaderRV);
		DEV_RELEASE(mpTexture);
		mpTextureSamplerState = nullptr;
	}

	bool CDX11TextureArray::addTexture(const std::string& szTexture, bool bGenerateMips/* = false*/)
	{
		auto strList = Foundation::splitString<std::string>(szTexture, " ");
		mTextureCount = strList.size();
		std::vector<ID3D11Texture2D*> srcTexVec(mTextureCount);
		std::vector<D3D11_TEXTURE2D_DESC> texDescVec(mTextureCount);

		CDevice* pDevice = GEngine->getDevice();
		auto pDeviceContext = pDevice->getDeviceContext();
		try
		{
			GEngine->commitCommand(commandType::StopDraw);
			for (int i = 0; i < mTextureCount; ++i)
			{
				D3DX11_IMAGE_LOAD_INFO loadInfo;
				ZeroMemory(&loadInfo, sizeof(D3DX11_IMAGE_LOAD_INFO));
				loadInfo.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				loadInfo.Usage = D3D11_USAGE_STAGING;
				loadInfo.CpuAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;

				HRESULT result = D3DX11CreateTextureFromFile(pDevice->getGraphicsDevice(),
					strList[i].c_str(), &loadInfo, nullptr, reinterpret_cast<ID3D11Resource**>(&srcTexVec[i]), nullptr);

				if (FAILED(result))
				{
					throw - 1;
				}

				srcTexVec[i]->GetDesc(&texDescVec[i]);
				if (texDescVec[i].MipLevels != texDescVec[0].MipLevels || texDescVec[i].Width != texDescVec[0].Width ||
					texDescVec[i].Height != texDescVec[0].Height || texDescVec[i].Format != texDescVec[0].Format)
				{
					for (int j = 0; j < i; ++j)
					{
						DEV_RELEASE(srcTexVec[j]);
					}
					throw - 1;
				}
			}
			D3D11_TEXTURE2D_DESC texArrayDesc;
			texArrayDesc.Width = texDescVec[0].Width;
			texArrayDesc.Height = texDescVec[0].Height;
			texArrayDesc.MipLevels = bGenerateMips ? 0 : texDescVec[0].MipLevels;
			texArrayDesc.ArraySize = mTextureCount;
			texArrayDesc.Format = texDescVec[0].Format;
			texArrayDesc.SampleDesc.Count = 1;
			texArrayDesc.SampleDesc.Quality = 0;
			texArrayDesc.Usage = D3D11_USAGE_DEFAULT;
			texArrayDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | (bGenerateMips ? D3D11_BIND_RENDER_TARGET : 0);
			texArrayDesc.CPUAccessFlags = 0;
			texArrayDesc.MiscFlags = (bGenerateMips ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0);

			HRESULT hr = pDevice->getGraphicsDevice()->CreateTexture2D(&texArrayDesc, nullptr, &mpTexture);
			if (FAILED(hr))
			{
				DEV_RELEASE(mpTexture);
				throw 0;
			}

			UINT updateMipLevels = bGenerateMips ? 1 : texArrayDesc.MipLevels;
			D3D11_MAPPED_SUBRESOURCE mappedTex2D;

			for (int i = 0; i < mTextureCount; ++i)
			{
				auto pTexture = srcTexVec[i];
				for (UINT j = 0; j < updateMipLevels; ++j)
				{
					hr = pDeviceContext->Map(pTexture, j, D3D11_MAP_READ, 0, &mappedTex2D);
					pDeviceContext->UpdateSubresource(mpTexture,
						D3D11CalcSubresource(j, i, texArrayDesc.MipLevels),	// i * mipLevel + j
						nullptr, mappedTex2D.pData, mappedTex2D.RowPitch, mappedTex2D.DepthPitch);
					pDeviceContext->Unmap(pTexture, j);
				}
				DEV_RELEASE(pTexture);
			}
			srcTexVec.clear();

			if (nullptr != mpTexture)
			{
				// 获取实际创建的纹理数组信息
				mpTexture->GetDesc(&texArrayDesc);
				D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
				viewDesc.Format = texArrayDesc.Format;
				viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
				viewDesc.Texture2DArray.MostDetailedMip = 0;
				viewDesc.Texture2DArray.MipLevels = -1;
				viewDesc.Texture2DArray.FirstArraySlice = 0;
				viewDesc.Texture2DArray.ArraySize = mTextureCount;

				hr = pDevice->getGraphicsDevice()->CreateShaderResourceView(mpTexture, &viewDesc, &mShaderRV);
				if (FAILED(hr))
				{
					DEV_RELEASE(mShaderRV);
					DEV_RELEASE(mpTexture);
					throw - 1;
				}

				// 生成mipmaps
				if (bGenerateMips)
				{
					pDeviceContext->GenerateMips(mShaderRV);
				}
			}
		}
		catch (...)
		{
			GEngine->commitCommand(commandType::RecoverDraw);
			return false;
		}
		GEngine->commitCommand(commandType::RecoverDraw);
		return true;
	}

	void CDX11TextureArray::addTexture(CTexture* pTexture)
	{
		CDX11Texture* pDX11Texture = dynamic_cast<CDX11Texture*>(pTexture);
		if (nullptr != pDX11Texture)
		{
			mShaderRVVector.push_back(pDX11Texture->mpShaderResourceView);
			mSampleStateVector.push_back(pDX11Texture->mpSampleState);
			mTextureVector.push_back(pDX11Texture);
			pDX11Texture->addRef();
			++mTextureCount;
		}
	}

	void CDX11TextureArray::removeTexture(CTexture* pTexture)
	{
		auto iter = std::find(mTextureVector.begin(), mTextureVector.end(), pTexture);
		for (size_t i = 0; i < mTextureVector.size(); ++i)
		{
			if (mTextureVector[i] == pTexture)
			{
				removeTexture(i);
				break;
			}
		}
	}

	void CDX11TextureArray::removeTexture(size_t nIndex)
	{
		if (mTextureVector.size() < nIndex)
		{
			--mTextureCount;
			{
				auto iter = mTextureVector.begin() + nIndex;
				mTextureVector.erase(iter);
			}
			{
				auto iter = mShaderRVVector.begin() + nIndex;
				mShaderRVVector.erase(iter);
			}
			{
				auto iter = mSampleStateVector.begin() + nIndex;
				mSampleStateVector.erase(iter);
			}
		}
	}

	void CDX11TextureArray::clearAllTextures()
	{
		for (int i = 0; i < mTextureCount; ++i)
		{
			removeTexture(i);
		}
	}

	void CDX11TextureArray::setSamplerState(SamplerState* pSampleState)
	{
		mpTextureSamplerState = pSampleState;
	}

	void CDX11TextureArray::operator=(const CDX11TextureArray& textureaArray)
	{
		clearAllTextures();
		mCount = textureaArray.mCount;
		mTextureCount = textureaArray.mTextureCount;
		mpTexture = textureaArray.mpTexture;
		mShaderRV = textureaArray.mShaderRV;
		mpTextureSamplerState = textureaArray.mpTextureSamplerState;
		if (nullptr != mpTexture)
		{
			mpTexture->AddRef();
		}
		if (nullptr != mShaderRV)
		{
			mShaderRV->AddRef();
		}
		for (auto item : textureaArray.mTextureVector)
		{
			item->addRef();
			mTextureVector.push_back(item);
		}
		for (auto item : textureaArray.mShaderRVVector)
		{
			mShaderRVVector.push_back(item);
		}
		for (auto item : textureaArray.mSampleStateVector)
		{
			mSampleStateVector.push_back(item);
		}
	}

}