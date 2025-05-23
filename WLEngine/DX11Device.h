#pragma once

#include "DX11UnDef.h"
#include <d3dcommon.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <DxErr.h>
#include <d3dcompiler.h>
#include <dxgi.h>

#include "Win.h"
#include "DeviceBase.h"


#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "dxerr.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "legacy_stdio_definitions.lib")


typedef D3D11_INPUT_ELEMENT_DESC		INPUT_ELEMENT_DESC;
typedef ID3D11SamplerState				SamplerState;
//typedef D3D11_SAMPLER_DESC				SAMPLER_DESC;
typedef D3D11_TEXTURE_ADDRESS_MODE		TEXTURE_ADDRESS_MODE;
typedef D3D11_FILTER					FILTER;
//typedef D3D11_COMPARISON_FUNC			COMPARISON_FUNC;
typedef D3D11_USAGE						USAGE;

#define DeviceContext ID3D11DeviceContext
#define CommandList ID3D11CommandList
#define DepthStencilView ID3D11DepthStencilView
#define RenderTargetView ID3D11RenderTargetView
#define SamplerState ID3D11SamplerState 
#define GFXDevice ID3D11Device

namespace WL
{
	class CDX11Device :  public CDeviceBase
	{
		friend class CGraphicEngine;
	public:
		CDX11Device();
		virtual ~CDX11Device();
		ID3D11Device* getGraphicsDevice();
		ID3D11DeviceContext* getDeviceContext(int index = -1);
		ID3D11DeviceContext* getImmediateContext();
		const D3D11_VIEWPORT& getViewPort()const;
		ID3D11DepthStencilView* getDepthStencilView()const;
		ID3D11RenderTargetView* getRenderTargetView()const;
		virtual SRenderState* createRenderState(SRenderStateDesc& renderStateDesc) final;
		bool concurrentCreates();
		bool createBuffer(D3D11_BUFFER_DESC* pVbDesc, D3D11_SUBRESOURCE_DATA* pData, ID3D11Buffer** pBuffer);

	protected:
		bool initDevice();
		int  getDeferredContextCount();
		bool beginDrawScene();
		bool endDrawScene();
		bool restoreRenderTargets();

	private:
		ID3D11Device* mpDevice = nullptr;
		IDXGISwapChain* m_CurrentSwapChain = nullptr;
		ID3D11DeviceContext* mImmediateContext = nullptr;
		ID3D11RenderTargetView* mpRenderTargetView = nullptr;
		ID3D11Texture2D* mpDepthStencilBuffer = nullptr;
		ID3D11DepthStencilView* mpDepthStencilView = nullptr;

		ID3D11Texture2D* mpBackBuffer = nullptr;
		D3D11_VIEWPORT	mViewport;
		D3D11_FEATURE_DATA_THREADING mThreadingFeature;
		std::vector<ID3D11DeviceContext*> mDeferredContexts;
		std::map<HWND, IDXGISwapChain*>	mMultipleWindowSwapChain;
		std::map<HWND, ID3D11RenderTargetView*>	mRenderTargetViews;
	};
}