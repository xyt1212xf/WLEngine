#include "DX11Device.h"
#include "WLEngine.h"
#include "MathLib.h"
#include "RenderStateMgr.h"

namespace WL
{
	CDX11Device::CDX11Device()
	{
	}

	CDX11Device::~CDX11Device()
	{
		mpRenderTargetView = nullptr;

		DEV_RELEASE(m_CurrentSwapChain);
		DEV_RELEASE(mImmediateContext);
		DEV_RELEASE(mpDepthStencilBuffer);
		DEV_RELEASE(mpDepthStencilView);
		DEV_RELEASE(mpBackBuffer);

		WL_DELETE(mpRenderStateMgr, RenderState);

		for (auto item : mMultipleWindowSwapChain)
		{
			DEV_RELEASE(item.second);
		}
		for (auto item : mRenderTargetViews)
		{
			DEV_RELEASE(item.second);
		}
		for (auto item : mDeferredContexts)
		{
			DEV_RELEASE(item);
		}
#ifdef _DEBUG
		ID3D11Debug* d3dDebug = nullptr;
		HRESULT hr = mpDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&d3dDebug));
		if (SUCCEEDED(hr))
		{
			hr = d3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
			DEV_RELEASE(d3dDebug);
		}
#endif
		DEV_RELEASE(mpDevice);
	}

	ID3D11Device* CDX11Device::getGraphicsDevice()
	{
		return mpDevice;
	}

	ID3D11DeviceContext* CDX11Device::getDeviceContext(int index /*= -1*/)
	{
		if (-1 == index)
		{
			return mImmediateContext;
		}
		else
		{
			if (index < (int)mDeferredContexts.size())
			{
				return mDeferredContexts[index];
			}
		}
		return nullptr;
	}

	ID3D11DeviceContext* CDX11Device::getImmediateContext()
	{
		return mImmediateContext;
	}

	const D3D11_VIEWPORT& CDX11Device::getViewPort() const
	{
		return mViewport;
	}

	ID3D11DepthStencilView* CDX11Device::getDepthStencilView() const
	{
		return mpDepthStencilView;
	}

	ID3D11RenderTargetView* CDX11Device::getRenderTargetView() const 
	{
		return mpRenderTargetView;
	}

	bool CDX11Device::initDevice()
	{
		bool bInitSucceeded = false;
		if (nullptr != GEngine)
		{
			HWND hWnd = GEngine->getMainWnd();
			if (nullptr != hWnd)
			{
				mpConfig = GEngine->getWindowConfigPtr();
				HRESULT result;
				unsigned int numModes = 0;
				unsigned int numerator = 0;
				unsigned int denominator = 0;
				IDXGIFactory* factory = nullptr;
				IDXGIAdapter* adapter = nullptr;
				IDXGIOutput* adapterOutput = nullptr;
				DXGI_MODE_DESC* displayModeList = nullptr;
				DXGI_ADAPTER_DESC adapterDesc;
				DXGI_SWAP_CHAIN_DESC swapChainDesc;
				D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
				D3D11_TEXTURE2D_DESC depthBufferDesc;
				D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
				do
				{
					// Create a DirectX graphics interface factory.
					result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
					if (FAILED(result))
					{
						continue;
					}

					// Use the factory to create an adapter for the primary graphics interface (video card).
					result = factory->EnumAdapters(0, &adapter);
					if (FAILED(result))
					{
						continue;
					}

					// Enumerate the primary adapter output (monitor).
					result = adapter->EnumOutputs(0, &adapterOutput);
					if (FAILED(result))
					{
						continue;
					}

					// Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
					result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
					if (FAILED(result))
					{
						continue;
					}

					// Create a list to hold all the possible display modes for this monitor/video card combination.
					displayModeList = WL_NEW_ARRAY(DXGI_MODE_DESC, numModes);
					if (!displayModeList)
					{
						continue;
					}

					// Now fill the display mode list structures.
					result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
					if (FAILED(result))
					{
						continue;
					}


					// Now go through all the display modes and find the one that matches the screen width and height.
					// When a match is found store the numerator and denominator of the refresh rate for that monitor.
					auto config = *GEngine->getWindowConfigPtr();
					for (int i = 0; i < (int)numModes; i++)
					{
						if (displayModeList[i].Width == (unsigned int)config.nWidth)
						{
							if (displayModeList[i].Height == (unsigned int)config.nHeight)
							{
								numerator = displayModeList[i].RefreshRate.Numerator;
								denominator = displayModeList[i].RefreshRate.Denominator;
							}
						}
					}

					// Get the adapter (video card) description.
					result = adapter->GetDesc(&adapterDesc);
					if (FAILED(result))
					{
						continue;
					}

					// Initialize the swap chain description.
					ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

					// Set to a single back buffer.
					swapChainDesc.BufferCount = 1;

					// Set the width and height of the back buffer.
					swapChainDesc.BufferDesc.Width = config.nWidth;
					swapChainDesc.BufferDesc.Height = config.nHeight;

					// Set regular 32-bit surface for the back buffer.
					swapChainDesc.BufferDesc.Format = static_cast<DXGI_FORMAT>(config.backBufferFormat);

					// Set the refresh rate of the back buffer.
					if (config.bVsync)
					{
						swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
						swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
					}
					else
					{
						swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
						swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
					}
					HWND hWnd = GEngine->getMainWnd();
					// Set the usage of the back buffer.
					swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;

					// Set the handle for the window to render to.
					swapChainDesc.OutputWindow = hWnd;

					// Turn multisampling off.
					swapChainDesc.SampleDesc.Count = 1;
					swapChainDesc.SampleDesc.Quality = 0;


					// Set to full screen or windowed mode.
					swapChainDesc.Windowed = !config.bFullScreen;

					// Set the scan line ordering and scaling to unspecified.
					swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
					swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

					// Discard the back buffer contents after presenting.
					swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

					// Don't set the advanced flags.
					swapChainDesc.Flags = 0;

					// Set the feature level to DirectX 11.
					UINT32 creationFlags = 0;// D3D11_CREATE_DEVICE_SINGLETHREADED;
#ifdef _DEBUG
					creationFlags = D3D11_CREATE_DEVICE_DEBUG;
#endif // DEBUG

					// Create the swap chain, Direct3D device, and Direct3D device context.
					result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, creationFlags, &featureLevel, 1,
						D3D11_SDK_VERSION, &swapChainDesc, &m_CurrentSwapChain, &mpDevice, NULL, &mImmediateContext);
					if (FAILED(result))
					{
						continue;
					}
					result = m_CurrentSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&mpBackBuffer);
					if (FAILED(result))
					{
						continue;
					}
					//m_CurrentSwapChain->Release();
					mpRenderStateMgr = WL_NEW(CRenderStateMgr, RenderState)(dynamic_cast<CDevice*>(this));   

					mMultipleWindowSwapChain[hWnd] = m_CurrentSwapChain;

					// Create the render target view with the back buffer pointer.
					result = mpDevice->CreateRenderTargetView(mpBackBuffer, NULL, &mpRenderTargetView);
					if (FAILED(result))
					{
						continue;
					}
					mRenderTargetViews[hWnd] = mpRenderTargetView;

					// Initialize the description of the depth buffer.
					ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

					// Set up the description of the depth buffer.
					depthBufferDesc.Width = config.nWidth;
					depthBufferDesc.Height = config.nHeight;
					depthBufferDesc.MipLevels = 1;
					depthBufferDesc.ArraySize = 1;
					depthBufferDesc.Format = static_cast<DXGI_FORMAT>(config.depthBufferFormat);
					depthBufferDesc.SampleDesc.Count = 1;
					depthBufferDesc.SampleDesc.Quality = 0;
					depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
					depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
					depthBufferDesc.CPUAccessFlags = 0;
					depthBufferDesc.MiscFlags = 0;

					// Create the texture for the depth buffer using the filled out description.
					result = mpDevice->CreateTexture2D(&depthBufferDesc, NULL, &mpDepthStencilBuffer);
					if (FAILED(result))
					{
						return false;
					}

					// Initialize the depth stencil view.
					ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

					// Set up the depth stencil view description.
					depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
					depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
					depthStencilViewDesc.Texture2D.MipSlice = 0;

					// Create the depth stencil view.
					result = mpDevice->CreateDepthStencilView(mpDepthStencilBuffer, &depthStencilViewDesc, &mpDepthStencilView);
					if (FAILED(result))
					{
						return false;
					}	
					mImmediateContext->OMSetRenderTargets(1, &mpRenderTargetView, mpDepthStencilView);
					RenderStateDesc RS;
					RS.stencil = true;
					mpRenderStateMgr->setDepthStencil(mImmediateContext, &RS);

					RS.reset();
					RS.fillMode = FILL_SOLID;
					RS.cullMode = CULL_NONE;
					mpRenderStateMgr->setFillCullMode(mImmediateContext, &RS);

					int nCpuCoreCount = std::thread::hardware_concurrency() / 2;
					int i = 0;
					for (; i < nCpuCoreCount; ++i)
					{
						ID3D11DeviceContext* pContext = nullptr;
						if (S_OK == mpDevice->CreateDeferredContext(0, &pContext))
						{
							mDeferredContexts.push_back(pContext);
						}
						else
						{
							break;
						}
					}
					mViewport.Width = (float)config.nWidth;
					mViewport.Height = (float)config.nHeight;
					mViewport.MinDepth = 0.0f;
					mViewport.MaxDepth = 1.0f;
					mViewport.TopLeftX = 0.0f;
					mViewport.TopLeftY = 0.0f;

					// Create the viewport.
					mImmediateContext->RSSetViewports(1, &mViewport);
					
					// Create the ScissorRects;
					RECT rect = { 0, 0, static_cast<int>(mViewport.Width), static_cast<int>(mViewport.Height) };
					mRT.top = rect.top;
					mRT.bottom = rect.bottom;

					mRT.left = rect.left;
					mRT.right = rect.right;

					mImmediateContext->RSSetScissorRects(1, &rect);

					if (i == nCpuCoreCount)
					{
						bInitSucceeded = true;
					}
					else
					{
						bInitSucceeded = false;
					}
				} while (false);
				// Release the display mode list.
				WL_DELETE_ARRAY(displayModeList);

				// Release the adapter output.
				adapterOutput->Release();

				// Release the adapter.
				adapter->Release();

				// Release the factory.
				factory->Release();
				ZeroMemory(&mThreadingFeature, sizeof(D3D11_FEATURE_DATA_THREADING));
				mpDevice->CheckFeatureSupport(D3D11_FEATURE_THREADING, &mThreadingFeature, sizeof(D3D11_FEATURE_DATA_THREADING));
				//mpDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &numQualityLevels);

			}
		}
	
		return bInitSucceeded;
	}

	int CDX11Device::getDeferredContextCount()
	{
		return static_cast<int>(mDeferredContexts.size());
	}

	bool CDX11Device::beginDrawScene()
	{
		static float color[4] = { 0,0,0,1.0f };
		// Clear the back buffer.
		mImmediateContext->ClearRenderTargetView(mpRenderTargetView, color);
		// Clear the DepthStencil buffer.
		mImmediateContext->ClearDepthStencilView(mpDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		return true;
	}

	bool CDX11Device::endDrawScene()
	{
		if (mpConfig->bVsync)
		{
			m_CurrentSwapChain->Present(0, 0);
		}
		else
		{
			m_CurrentSwapChain->Present(1, 0);
		}
		return true;
	}

	bool CDX11Device::restoreRenderTargets()
	{
		mImmediateContext->OMSetRenderTargets(1, &mpRenderTargetView, mpDepthStencilView);
		mImmediateContext->RSSetViewports(1, &mViewport);
		return true;
	}

	RenderState* CDX11Device::createRenderState(RenderStateDesc& renderStateDesc)
	{
		RenderState* pRenderState = nullptr;
		if (nullptr != mpRenderStateMgr)
		{
			pRenderState = mpRenderStateMgr->createRenderState(renderStateDesc);
		}
		return pRenderState;
	}

	bool CDX11Device::concurrentCreates()
	{
		return mThreadingFeature.DriverConcurrentCreates == 1 ? true : false;
	}

	bool CDX11Device::createBuffer(D3D11_BUFFER_DESC* pVbDesc, D3D11_SUBRESOURCE_DATA* pData, ID3D11Buffer** pBuffer)
	{
		auto hr = mpDevice->CreateBuffer(pVbDesc, pData, pBuffer);
		return hr == S_OK ? true : false;
	}

}