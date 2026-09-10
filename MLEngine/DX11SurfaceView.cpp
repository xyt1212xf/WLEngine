#include "DX11SurfaceView.h"
#include "WLEngine.h"

namespace WL
{
	CDX11SurfaceView::CDX11SurfaceView(ID3D11RenderTargetView* pRTTView, ID3D11DepthStencilView* pDepthStencil)
	: CSurfaceViewBase()
	{
		bindRenderTargetView(pRTTView);
		bindDepthStencilView(pDepthStencil);
	}

	CDX11SurfaceView::~CDX11SurfaceView()
	{
		DEV_RELEASE(mpRenderTargetView);
		DEV_RELEASE(mpRenderTexture);

		DEV_RELEASE(mpDepthStencilView);
		DEV_RELEASE(mpDepthStencilBuffer);

		DEV_RELEASE(mpUnorderedAccessView);
	 	DEV_RELEASE(mpShaderResourcView);
		DEV_RELEASE(mpBuffer);
	}

	bool CDX11SurfaceView::saveToFile(const std::string szFile)
	{
		if (nullptr != mpTexture)
		{
			return mpTexture->saveToFile(szFile);
		}
		else
		{
			return false;
		}
	}

	bool CDX11SurfaceView::createRenderSurface(int nWidth, int nHeight, UINT32 nTextureFormat, UINT32 depthFormat, UINT bindInfo, UINT32 miscFlags)
	{
		mWidth = nWidth;
		mHeight = nHeight;

		if (0 != depthFormat)
		{
			CDevice* pDevice = GEngine->getDevice();

			D3D11_TEXTURE2D_DESC depthBufferDesc;
			// Initialize the description of the depth buffer.
			ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

			// Set up the description of the depth buffer.
			depthBufferDesc.Width = mWidth;
			depthBufferDesc.Height = mHeight;
			depthBufferDesc.MipLevels = 1;
			depthBufferDesc.ArraySize = 1;
			depthBufferDesc.Format = (DXGI_FORMAT)depthFormat;// DXGI_FORMAT_D24_UNORM_S8_UINT;
			depthBufferDesc.SampleDesc.Count = 1;
			depthBufferDesc.SampleDesc.Quality = 0;
			depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			depthBufferDesc.BindFlags = bindInfo;// D3D11_BIND_DEPTH_STENCIL;
			depthBufferDesc.CPUAccessFlags = 0;
			depthBufferDesc.MiscFlags = 0;

			// Create the texture for the depth buffer using the filled out description.
			HRESULT result = pDevice->getGraphicsDevice()->CreateTexture2D(&depthBufferDesc, NULL, &mpDepthStencilBuffer);
			if (FAILED(result))
			{
				return false;
			}

			D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
			// Initialize the depth stencil view.
			ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

			// Set up the depth stencil view description.
			depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			depthStencilViewDesc.Texture2D.MipSlice = 0;

			// Create the depth stencil view.
			result = pDevice->getGraphicsDevice()->CreateDepthStencilView(mpDepthStencilBuffer, &depthStencilViewDesc, &mpDepthStencilView);
			if (FAILED(result))
			{
				return false;
			}
		}
		mpTexture = dynamic_cast<CTexture*>(GEngine->createResource(Texture));

		if (nullptr != mpTexture)
		{
			mpTexture->addRef();
			mpTexture->initialiseTexture2D(mWidth, mHeight, nTextureFormat, BIND_RENDER_TARGET | BIND_SHADER_RESOURCE, 0, D3D11_USAGE_DEFAULT, miscFlags);
			// Setup the viewport for rendering.
			mViewport.Width = (float)mWidth;
			mViewport.Height = (float)mHeight;
			mViewport.MinDepth = 0.0f;
			mViewport.MaxDepth = 1.0f;
			mViewport.TopLeftX = 0.0f;
			mViewport.TopLeftY = 0.0f;

			auto pTexture = mpTexture->getResourcePtr();
			if (S_OK == pTexture->QueryInterface(IID_ID3D11Texture2D, (void**)&mpRenderTexture))
			{
				mpRenderTargetView = mpTexture->getRenderTargetViewPtr();
				mpShaderResourcView = mpTexture->getShaderResViewPtr();
				return true;
			}
		}
		return false;
	}

	bool CDX11SurfaceView::createBuffer(void* pData,
		UINT32 nByteWidth, UINT32 nStructStride, UINT32 nUsage, UINT32 nCpuAccessFlag, UINT32 nBindFlag, UINT32 nMiscFlag)
	{
		CDevice* pDevice = GEngine->getDevice();
		// Create buffer
		D3D11_BUFFER_DESC buf_desc;
		buf_desc.ByteWidth = nByteWidth;
		buf_desc.Usage = static_cast<D3D11_USAGE>(nUsage);
		buf_desc.BindFlags = nBindFlag;
		buf_desc.CPUAccessFlags = nCpuAccessFlag;
		buf_desc.MiscFlags = nMiscFlag;
		buf_desc.StructureByteStride = nStructStride;

		D3D11_SUBRESOURCE_DATA init_data = { pData, 0, 0 };

		return S_OK == pDevice->getGraphicsDevice()->CreateBuffer(&buf_desc, pData != nullptr ? &init_data : nullptr, &mpBuffer);
	}

	bool CDX11SurfaceView::createUAVBuffer(void* pData, UINT32 nByteWidth, UINT32 nStructStride, UINT32 nUsage /*= USAGE_DEFAULT*/, UINT32 nCpuAccessFlag /*= 0*/, UINT32 nBindFlag /*= BIND_UNORDERED_ACCESS | BIND_SHADER_RESOURCE*/, UINT32 nMiscFlag /*= RESOURCE_MISC_BUFFER_STRUCTURED*/)
	{
		do 
		{
			if (createBuffer(pData, nByteWidth, nStructStride, nUsage, nCpuAccessFlag, nBindFlag, nMiscFlag))
			{
				CDevice* pDevice = GEngine->getDevice();
				// Create undordered access view
				D3D11_UNORDERED_ACCESS_VIEW_DESC uav_desc;
				uav_desc.Format = DXGI_FORMAT_UNKNOWN;
				uav_desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
				uav_desc.Buffer.FirstElement = 0;
				uav_desc.Buffer.NumElements = nByteWidth / nStructStride;
				uav_desc.Buffer.Flags = 0;

				if (S_OK != pDevice->getGraphicsDevice()->CreateUnorderedAccessView(mpBuffer, &uav_desc, &mpUnorderedAccessView))
				{
					break;
				}
				else
				{
					// Create shader resource view
					D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
					srv_desc.Format = DXGI_FORMAT_UNKNOWN;
					srv_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
					srv_desc.Buffer.FirstElement = 0;
					srv_desc.Buffer.NumElements = nByteWidth / nStructStride;

					if (S_OK != pDevice->getGraphicsDevice()->CreateShaderResourceView(mpBuffer, &srv_desc, &mpShaderResourcView))
					{
						break;
					}
				}
			}
			else
			{
				break;
			}
			return true;
		} while( false );
		return false;
	}

	void CDX11SurfaceView::clearRenderSurface(DeviceContext* pContext)
	{
		// Clear the depth buffer.
		if (nullptr != mpDepthStencilView)
		{
			pContext->ClearDepthStencilView(mpDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
		}

		// Clear the renderTaget buffer.
		if (nullptr != mpTexture)
		{
			mpTexture->clearRenderTargetView(pContext, BLACK);
		}
	}

	void CDX11SurfaceView::setRenderSurface(DeviceContext* pContext)
	{
		// Bind the render target view and depth stencil buffer to the output render pipeline.
		ID3D11RenderTargetView* rt_views[1] = { mpRenderTargetView };
		pContext->OMSetRenderTargets(1, rt_views, mpDepthStencilView);

		// Set the viewport.
		pContext->RSSetViewports(1, &mViewport);
	}

	void CDX11SurfaceView::bindDepthStencilView(ID3D11DepthStencilView* pDepthStencilView)
	{
		DEV_RELEASE(mpDepthStencilView);
		mpDepthStencilView = pDepthStencilView;
		mpDepthStencilView->AddRef();
	}

	void CDX11SurfaceView::bindRenderTargetView(ID3D11RenderTargetView* pRenderTargetView)
	{
		DEV_RELEASE(mpRenderTargetView);
		mpRenderTargetView = pRenderTargetView;
		mpRenderTargetView->AddRef();
	}

	ID3D11Texture2D* CDX11SurfaceView::getRenderTexturePtr() 
	{
		return mpRenderTexture;
	}

	ID3D11RenderTargetView* CDX11SurfaceView::getRenderTargetViewPtr() const
	{
		return mpRenderTargetView;
	}

	ID3D11Texture2D* CDX11SurfaceView::getDepthTexturePtr() const
	{
		return mpDepthStencilBuffer;
	}

	ID3D11DepthStencilView* CDX11SurfaceView::getDepthStencilViewPtr() const
	{
		return mpDepthStencilView;
	}

	D3D11_VIEWPORT& CDX11SurfaceView::getViewPort()
	{
		return mViewport;
	}

	D3D11_VIEWPORT* CDX11SurfaceView::getViewPortPtr() 
	{
		return &mViewport;
	}

	ID3D11UnorderedAccessView* CDX11SurfaceView::getUnorderedAccessViewPtr() const
	{
		return mpUnorderedAccessView;
	}

	ID3D11Buffer* CDX11SurfaceView::getBufferPtr() const
	{
		return mpBuffer;
	}

	ID3D11ShaderResourceView* CDX11SurfaceView::getShaderResourceView() const
	{
		return mpShaderResourcView;
	}

	void CDX11SurfaceView::setVShaderResource(DeviceContext* pContext, int nStart)
	{
		ID3D11ShaderResourceView* srvs[1] = { mpShaderResourcView };
		pContext->VSSetShaderResources(nStart, 1, srvs);
	}

	void CDX11SurfaceView::clearVSShaderResource(DeviceContext* pContext) const
	{
		ID3D11ShaderResourceView* srvs[1] = { nullptr };
		pContext->VSSetShaderResources(mUAVStart, 1, srvs);
	}

	void CDX11SurfaceView::setPShaderResource(DeviceContext* pContext, int nStart)
	{
		ID3D11ShaderResourceView* srvs[1] = { mpShaderResourcView };
		pContext->PSSetShaderResources(nStart, 1, srvs);
	}

	void CDX11SurfaceView::clearPSShaderResource(DeviceContext* pContext) const
	{
		ID3D11ShaderResourceView* srvs[1] = { nullptr };
		pContext->PSSetShaderResources(mUAVStart, 1, srvs);
	}

	void CDX11SurfaceView::setGShaderResource(DeviceContext* pContext, int nStart)
	{
		ID3D11ShaderResourceView* srvs[1] = { mpShaderResourcView };
		pContext->GSSetShaderResources(nStart, 1, srvs);
	}

	void CDX11SurfaceView::clearGSShaderResource(DeviceContext* pContext) const
	{
		ID3D11ShaderResourceView* srvs[1] = { nullptr };
		pContext->GSSetShaderResources(mUAVStart, 1, srvs);
	}

	void CDX11SurfaceView::setHShaderResource(DeviceContext* pContext, int nStart)
	{
		ID3D11ShaderResourceView* srvs[1] = { mpShaderResourcView };
		pContext->HSSetShaderResources(nStart, 1, srvs);
	}

	void CDX11SurfaceView::clearHSShaderResource(DeviceContext* pContext) const
	{
		ID3D11ShaderResourceView* srvs[1] = { nullptr };
		pContext->HSSetShaderResources(mUAVStart, 1, srvs);
	}

	void CDX11SurfaceView::setDShaderResource(DeviceContext* pContext, int nStart)
	{
		ID3D11ShaderResourceView* srvs[1] = { mpShaderResourcView };
		pContext->DSSetShaderResources(nStart, 1, srvs);
	}

	void CDX11SurfaceView::clearDSShaderResource(DeviceContext* pContext) const
	{
		ID3D11ShaderResourceView* srvs[1] = { nullptr };
		pContext->DSSetShaderResources(mUAVStart, 1, srvs);
	}

	void CDX11SurfaceView::setCSUAVSurface(DeviceContext* pContext, int nStart)
	{
		ID3D11UnorderedAccessView* uavs[1] = { mpUnorderedAccessView };
		mUAVStart = nStart;
		pContext->CSSetUnorderedAccessViews(nStart, 1, uavs, (UINT*)(&uavs[0]));
	}

	void CDX11SurfaceView::setCShaderResource(DeviceContext* pContext, int nStart)
	{
		ID3D11ShaderResourceView* srvs[1] = { mpShaderResourcView };
		pContext->CSSetShaderResources(nStart, 1, srvs);
	}

	void CDX11SurfaceView::setCSConstBuffer(DeviceContext* pContext, int nStart)
	{
		ID3D11Buffer* cs_buffer[1] = { mpBuffer };
		pContext->CSSetConstantBuffers(0, 1, &cs_buffer[0]);
	}

	void CDX11SurfaceView::clearCSUAVSurface(DeviceContext* pContext) const
	{
		ID3D11UnorderedAccessView* cs0_uavs[1] = { nullptr };
		pContext->CSSetUnorderedAccessViews(mUAVStart, 1, cs0_uavs, (UINT*)(&cs0_uavs[0]));
	}

	void CDX11SurfaceView::clearCSShaderResource(DeviceContext* pContext) const
	{
		ID3D11ShaderResourceView* cs0_srvs[1] = {nullptr};
		pContext->CSSetShaderResources(mUAVStart, 1, cs0_srvs);
	}

	void CDX11SurfaceView::clearCSConstBuffer(DeviceContext* pContext) const
	{
		ID3D11Buffer* cs_buffer[1] = { nullptr };
		pContext->CSSetConstantBuffers(0, 1, cs_buffer);
	}

}