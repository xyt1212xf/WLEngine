#pragma once
//
//#include "DX11UnDef.h"
#include <d3dcommon.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
//#include "include/d3dx12/d3dx12.h"
//#include <D3D12SDKLayers.h> 
//#include <d3dx12.h>
//#include <DxErr.h>
//#include <d3dcompiler.h>
//#include <dxgi.h>

#include "Win.h"
#include "DeviceBase.h"


#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
//#pragma comment(lib, "d3dx11.lib")
//#pragma comment(lib, "dxerr.lib")
//#pragma comment(lib, "d3dcompiler.lib")
//#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "legacy_stdio_definitions.lib")
//
//
//typedef D3D11_INPUT_ELEMENT_DESC		INPUT_ELEMENT_DESC;
//typedef ID3D11SamplerState				SamplerState;
////typedef D3D11_SAMPLER_DESC				SAMPLER_DESC;
//typedef D3D11_TEXTURE_ADDRESS_MODE		TEXTURE_ADDRESS_MODE;
//typedef D3D11_FILTER					FILTER;
////typedef D3D11_COMPARISON_FUNC			COMPARISON_FUNC;
//typedef D3D11_USAGE						USAGE;
//
#define DeviceContext 
//#define CommandList ID3D11CommandList
//#define DepthStencilView ID3D11DepthStencilView
//#define RenderTargetView ID3D11RenderTargetView
//#define SamplerState ID3D11SamplerState 
//#define GFXDevice ID3D11Device
//
#define CommandList ID3D12CommandList
//#define DepthStencilView ID3D12DepthStencilView
//#define RenderTargetView ID3D12RenderTargetView
//#define SamplerState ID3D12SamplerState 
#define GFXDevice ID3D12Device
namespace WL
{
	class CDX12Device :  public CDeviceBase
	{
		friend class CGraphicEngine;
	public:
		CDX12Device();
		virtual ~CDX12Device();
//		ID3D11Device* getGraphicsDevice();
//		ID3D11DeviceContext* getDeviceContext(int index = -1);
//		ID3D11DeviceContext* getImmediateContext();
//		const D3D11_VIEWPORT& getViewPort()const;
//		ID3D11DepthStencilView* getDepthStencilView()const;
//		ID3D11RenderTargetView* getRenderTargetView()const;
//		virtual SRenderState* createRenderState(SRenderStateDesc& renderStateDesc) final;
//		bool concurrentCreates();
//		bool createBuffer(D3D11_BUFFER_DESC* pVbDesc, D3D11_SUBRESOURCE_DATA* pData, ID3D11Buffer** pBuffer);
//
	protected:
		bool initDevice();
//		int  getDeferredContextCount();
//		bool beginDrawScene();
//		bool endDrawScene();
//		bool restoreRenderTargets();
//
	private:
		void GetHardwareAdapter( _In_ IDXGIFactory1* pFactory, _Outptr_result_maybenull_ IDXGIAdapter1** ppAdapter,
								bool requestHighPerformanceAdapter = false);
	private:
		UINT32 mRtvDescriptorSize = 0;
		ID3D12Device* mpDevice = nullptr;
		ID3D12CommandQueue* mCommandQueue = nullptr;
		IDXGISwapChain3* mSwapChain = nullptr;
		ID3D12DescriptorHeap* mRtvHeap = nullptr;
		ID3D12CommandAllocator* mCommandAllocator = nullptr;
		ID3D12GraphicsCommandList* mCommandList = nullptr;
		std::vector<ID3D12Resource*> mRenderTargets;

		// Synchronization objects.
		UINT32 mFrameIndex = 0;
		HANDLE mFenceEvent;
		ID3D12Fence* mFence = nullptr;
		UINT64 mFenceValue = 0;
	};
}