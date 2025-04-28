#include "DX11Texture.h"
#include "WLEngine.h"

namespace WL
{

	CDX11Texture::CDX11Texture()
	{
		ZeroMemory(&mDesc1D, sizeof(D3D11_TEXTURE1D_DESC));
		ZeroMemory(&mDesc2D, sizeof(D3D11_TEXTURE2D_DESC));
		ZeroMemory(&mDesc3D, sizeof(D3D11_TEXTURE3D_DESC));
	}

	CDX11Texture::~CDX11Texture()
	{
		mpDeviceContext = nullptr;
		DEV_RELEASE(mpSampleState);
		DEV_RELEASE(mpShaderResourceView);
		DEV_RELEASE(mpTextureResource);
		DEV_RELEASE(mpRenderTargetView);
		DEV_RELEASE(mpTextureUAV);
	}

	void CDX11Texture::clearRenderTargetView(DeviceContext* pContext, const Vec4F& color)
	{
		if (nullptr != mpRenderTargetView)
		{
			pContext->ClearRenderTargetView(mpRenderTargetView, &color.x);
		}
	}

	void CDX11Texture::setSampleState(SamplerState* pSampleState)
	{
		if (mpSampleState != pSampleState)
		{
			DEV_RELEASE(mpSampleState);
		}
		if (nullptr != pSampleState)
		{
			mpSampleState = pSampleState;
			mpSampleState->AddRef();
		}
	}

	const SamplerState* CDX11Texture::getSampleState() const
	{
		return mpSampleState;
	}

	bool CDX11Texture::initialiseShaderResourceView()
	{
		if (mpShaderResourceView == nullptr)
		{
			initialise();
		}
		return true;
	}

	bool CDX11Texture::initialiseTexture2D(INT32 width, INT32 height, UINT32 nFormat, UINT32 nBindFlag, UINT32 cpuAccessFlag, int usagFlag, UINT32 miscFlags)
	{
		CDevice* pDevice = GEngine->getDevice();
		mpDeviceContext = pDevice->getDeviceContext();

		D3D11_TEXTURE2D_DESC textureDesc;
		// Initialize the render target texture description.
		ZeroMemory(&textureDesc, sizeof(textureDesc));

		// Setup the render target texture description.
		textureDesc.Width = width;
		textureDesc.Height = height;
		textureDesc.MipLevels = 0;
		textureDesc.ArraySize = 1;
		textureDesc.Format = (DXGI_FORMAT)nFormat; 
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = (D3D11_USAGE)usagFlag;//;
		textureDesc.BindFlags = nBindFlag;
		textureDesc.CPUAccessFlags = cpuAccessFlag;
		textureDesc.MiscFlags = miscFlags;

		// Create the render target texture.
		ID3D11Texture2D* pTexture = nullptr;
		HRESULT result = pDevice->getGraphicsDevice()->CreateTexture2D(&textureDesc, nullptr, &pTexture);
		if (FAILED(result))
		{
			return false;
		}
		//mDesc2D = textureDesc;
		mpTextureResource = pTexture;
		pTexture->GetDesc(&mDesc2D);
		do
		{
			if (nBindFlag & D3D11_BIND_SHADER_RESOURCE)
			{
				D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

				// Setup the description of the shader resource view.
				shaderResourceViewDesc.Format = textureDesc.Format;
				shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				shaderResourceViewDesc.Texture2D.MipLevels = 1;// textureDesc.MipLevels;
				shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;

				// Create the shader resource view.
				result = pDevice->getGraphicsDevice()->CreateShaderResourceView(mpTextureResource, &shaderResourceViewDesc, &mpShaderResourceView);
				if (FAILED(result))
				{
					break;
				}
			}
			if (nBindFlag & D3D11_BIND_RENDER_TARGET)
			{
				D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
				// Setup the description of the render target view.
				renderTargetViewDesc.Format = textureDesc.Format;
				renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
				renderTargetViewDesc.Texture2D.MipSlice = 0;

				// Create the render target view.
				result = pDevice->getGraphicsDevice()->CreateRenderTargetView(mpTextureResource, &renderTargetViewDesc, &mpRenderTargetView);
				if (FAILED(result))
				{
					break;
				}
			}

			return true;
		} while (false);
		return false;
	}

	bool CDX11Texture::initialiseTexture1D(INT32 width, UINT32 nFormat, UINT32 nBindFlag, UINT32 cpuAccessFlag, int usagFlag, void* pData)
	{
		do
		{
			CDevice* pDevice = GEngine->getDevice();
			D3D11_TEXTURE1D_DESC tex_desc;
			tex_desc.Width = width;
			tex_desc.MipLevels = 1;
			tex_desc.ArraySize = 1;
			tex_desc.Format = static_cast<DXGI_FORMAT>(nFormat);
			tex_desc.Usage = static_cast<D3D11_USAGE>(usagFlag);
			tex_desc.BindFlags = static_cast<D3D11_BIND_FLAG>(nBindFlag);
			tex_desc.CPUAccessFlags = cpuAccessFlag;
			tex_desc.MiscFlags = 0;
			ID3D11Texture1D* pTexture = nullptr;
			HRESULT result = S_OK;
			if (nullptr != pData)
			{
				D3D11_SUBRESOURCE_DATA init_data;
				init_data.pSysMem = pData;
				init_data.SysMemPitch = 0;
				init_data.SysMemSlicePitch = 0;

				result = pDevice->getGraphicsDevice()->CreateTexture1D(&tex_desc, &init_data, &pTexture);
			}
			else
			{
				result = pDevice->getGraphicsDevice()->CreateTexture1D(&tex_desc, nullptr, &pTexture);
			}
			if (FAILED(result))
			{
				break;
			}
			mpTextureResource = pTexture;
			if (nBindFlag & D3D11_BIND_SHADER_RESOURCE)
			{
				D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

				// Setup the description of the shader resource view.
				shaderResourceViewDesc.Format = tex_desc.Format;
				shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
				shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
				shaderResourceViewDesc.Texture2D.MipLevels = 1;

				// Create the shader resource view.
				HRESULT result = pDevice->getGraphicsDevice()->CreateShaderResourceView(mpTextureResource, &shaderResourceViewDesc, &mpShaderResourceView);
				if (FAILED(result))
				{
					break;
				}
			}
			return true;
		} while (false);
		return false;
	}

	bool CDX11Texture::initialiseUnorderedTexture(INT32 width, INT32 height, UINT32 nFormat, UINT32 nFlag, UINT32 nBindFlag, UINT32 cpuAccessFlag, int usagFlag, UINT mipSlice /*= 0*/, UINT32 firstArraySlice /*= 0*/, UINT32 arraySize /*= -1*/)
	{
		CDevice* pDevice = GEngine->getDevice();
		mpDeviceContext = pDevice->getDeviceContext();
		// Initialize the render target texture description.
		D3D11_TEXTURE2D_DESC textureDesc;
		ZeroMemory(&textureDesc, sizeof(textureDesc));

		// Setup the render target texture description.
		textureDesc.Width = width;
		textureDesc.Height = height;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = (DXGI_FORMAT)nFormat;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Usage = (D3D11_USAGE)usagFlag;
		textureDesc.BindFlags = nFlag;
		textureDesc.CPUAccessFlags = cpuAccessFlag;
		textureDesc.MiscFlags = 0;

		ID3D11Texture2D* pTexture = nullptr;
		pDevice->getGraphicsDevice()->CreateTexture2D(&textureDesc, nullptr, &pTexture);
		mpTextureResource = pTexture;

		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
		ZeroMemory(&uavDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		uavDesc.Format = (DXGI_FORMAT)nFormat;

		switch (uavDesc.ViewDimension)
		{
		case D3D11_UAV_DIMENSION_BUFFER:
			uavDesc.Buffer.FirstElement = mipSlice;
			uavDesc.Buffer.NumElements = firstArraySlice;
			uavDesc.Buffer.Flags = nBindFlag;
			break;
		case D3D11_UAV_DIMENSION_TEXTURE1D:
			uavDesc.Texture1D.MipSlice = mipSlice;
			break;
		case D3D11_UAV_DIMENSION_TEXTURE1DARRAY:
			uavDesc.Texture1DArray.MipSlice = mipSlice;
			uavDesc.Texture1DArray.FirstArraySlice = firstArraySlice;
			uavDesc.Texture1DArray.ArraySize = arraySize;
			break;
		case D3D11_UAV_DIMENSION_TEXTURE2D:
			uavDesc.Texture2D.MipSlice = mipSlice;
			break;
		case D3D11_UAV_DIMENSION_TEXTURE2DARRAY:
			uavDesc.Texture2DArray.MipSlice = mipSlice;
			uavDesc.Texture2DArray.FirstArraySlice = firstArraySlice;
			uavDesc.Texture2DArray.ArraySize = arraySize;
			break;
		case D3D11_UAV_DIMENSION_TEXTURE3D:
			uavDesc.Texture3D.MipSlice = mipSlice;
			uavDesc.Texture3D.FirstWSlice = firstArraySlice;
			uavDesc.Texture3D.WSize = arraySize;
			break;
		default:
			break;
		}
		HRESULT hr = pDevice->getGraphicsDevice()->CreateUnorderedAccessView(mpTextureResource, &uavDesc, &mpTextureUAV);
		return S_OK == hr ? true : false;
	}

	bool CDX11Texture::initialise()
	{
		if (mResourceName.length() > 1)
		{
			CDevice* pDevice = GEngine->getDevice();
			mpDeviceContext = pDevice->getDeviceContext();

			UINT32 nMaxsize = D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION;
			D3D_FEATURE_LEVEL featureLevel = pDevice->getGraphicsDevice()->GetFeatureLevel();
			switch (featureLevel)
			{
			case D3D_FEATURE_LEVEL_9_1:
			case D3D_FEATURE_LEVEL_9_2:
				nMaxsize = 2048 /*D3D_FL9_1_REQ_TEXTURE2D_U_OR_V_DIMENSION*/;
				break;
			case D3D_FEATURE_LEVEL_9_3:
				nMaxsize = 4096 /*D3D_FL9_3_REQ_TEXTURE2D_U_OR_V_DIMENSION*/;
				break;
			case D3D_FEATURE_LEVEL_10_0:
			case D3D_FEATURE_LEVEL_10_1:
				nMaxsize = 8192 /*D3D10_REQ_TEXTURE2D_U_OR_V_DIMENSION*/;
				break;
			}
			D3DX11_IMAGE_LOAD_INFO loadInfo;
			ZeroMemory(&loadInfo, sizeof(D3DX11_IMAGE_LOAD_INFO));

			loadInfo.BindFlags = D3DX11_DEFAULT;
			loadInfo.Format = static_cast<DXGI_FORMAT>(mFileInfo.nFormat);
			loadInfo.MipLevels = D3DX11_DEFAULT;
			loadInfo.MipFilter = D3DX11_DEFAULT;
			loadInfo.Usage = D3D11_USAGE_DEFAULT;
			loadInfo.CpuAccessFlags = 0;

			do
			{
				HRESULT result = D3DX11CreateShaderResourceViewFromFile(pDevice->getGraphicsDevice(),
					mResourceName.c_str(), &loadInfo, nullptr, &mpShaderResourceView, nullptr);

				if (S_OK == result)
				{
					break;
				}
				if (loadInfo.Width > nMaxsize)
				{
					loadInfo.Width = nMaxsize;
				}
				if (loadInfo.Height > nMaxsize)
				{
					loadInfo.Height = nMaxsize;
				}
				result = D3DX11CreateShaderResourceViewFromFile(pDevice->getGraphicsDevice(),
					mResourceName.c_str(), &loadInfo, nullptr, &mpShaderResourceView, nullptr);

				if (S_OK != result)
				{
					return false;
				}
			} while (false);
			mpShaderResourceView->GetResource(&mpTextureResource);
			switch (mType)
			{
			case TextureType::D1:
			{
				ID3D11Texture1D* pTexture = nullptr;
				if (S_OK == mpTextureResource->QueryInterface(IID_ID3D11Texture1D, (void**)&pTexture))
				{
					pTexture->GetDesc(&mDesc1D);
					DEV_RELEASE(pTexture);
				}
			}
			break;
			case TextureType::D2:
			{
				ID3D11Texture2D* pTexture = nullptr;
				if (S_OK == mpTextureResource->QueryInterface(IID_ID3D11Texture2D, (void**)&pTexture))
				{
					pTexture->GetDesc(&mDesc2D);
					DEV_RELEASE(pTexture);
				}
			}
			break;
			case TextureType::D3:
			{
				ID3D11Texture3D* pTexture = nullptr;
				if (S_OK == mpTextureResource->QueryInterface(IID_ID3D11Texture3D, (void**)&pTexture))
				{
					pTexture->GetDesc(&mDesc3D);
					DEV_RELEASE(pTexture);
				}
			}
			break;
			}

#ifdef _DEBUG
			mpTextureResource->SetPrivateData(WKPDID_D3DDebugObjectName, mResourceName.length(), mResourceName.c_str());
#endif
			return true;
		}
		else
		{
			return false;
		}
	
	}

	bool CDX11Texture::initialise(const ImgFileInfo& fileInfo)
	{
		mFileInfo = fileInfo;
		mResourceName = mFileInfo.szPath;
		return initialise();
	}

	ID3D11RenderTargetView* CDX11Texture::getRenderTargetViewPtr() const
	{
		mpRenderTargetView->AddRef();
		return mpRenderTargetView;

	}

	ID3D11Resource* CDX11Texture::getResourcePtr() 
	{
		return mpTextureResource;
	}

	ID3D11ShaderResourceView* CDX11Texture::getShaderResViewPtr()
	{
		mpShaderResourceView->AddRef();
		return mpShaderResourceView;
	}

	ID3D11UnorderedAccessView* CDX11Texture::getUnordAccessViewPtr()
	{
		mpTextureUAV->AddRef();
		return mpTextureUAV;
	}

	bool CDX11Texture::saveToFile(const std::string saveFile /*= ""*/, DWORD nFormat /*= D3DX11_IFF_PNG*/)
	{
		try
		{
			if (nullptr != mpTextureResource)
			{
				ID3D11Texture2D* pTexture = nullptr;
				if (S_OK == mpTextureResource->QueryInterface(IID_ID3D11Texture2D, (void**)&pTexture))
				{
					CDevice* pDevice = GEngine->getDevice();
					D3D11_TEXTURE2D_DESC description;

					pTexture->GetDesc(&description);
					DEV_RELEASE(pTexture);

					description.BindFlags = 0;
					description.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
					description.Usage = D3D11_USAGE_STAGING;
					description.MiscFlags &= D3D11_RESOURCE_MISC_TEXTURECUBE;

					ID3D11Texture2D* texTemp = NULL;
					HRESULT hr = pDevice->getGraphicsDevice()->CreateTexture2D(&description, NULL, &texTemp);
					if (S_OK == hr)
					{
						pDevice->getDeviceContext()->CopyResource(texTemp, mpTextureResource);
						if ("" == saveFile)
						{
							hr = D3DX11SaveTextureToFile(mpDeviceContext,
								texTemp,
								static_cast<D3DX11_IMAGE_FILE_FORMAT>(nFormat),
								mResourceName.c_str());
						}
						else
						{
							hr = D3DX11SaveTextureToFile(mpDeviceContext,
								texTemp,
								static_cast<D3DX11_IMAGE_FILE_FORMAT>(nFormat),
								saveFile.c_str());
						}
						
						if (FAILED(hr))
						{
							throw texTemp;
						}
						DEV_RELEASE(texTemp);
					}
					else
					{
						throw hr;
					}
				}
			}
			return true;
		}
		catch (ID3D11Texture2D* pTex)
		{
			DEV_RELEASE(pTex);
			return false;
		}
		catch (...)
		{
			return false;
		}
	}
}