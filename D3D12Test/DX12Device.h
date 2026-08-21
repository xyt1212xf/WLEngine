#pragma once 
#include <d3d12.h>
#include "d3dx12.h"
#include <dxgi1_6.h>
#include <vector>
#include "Device.h"


namespace WL
{
	class CDX12Device : public CDeviceBase
	{
		friend class CGraphicEngine;
	public:
		CDX12Device();
		virtual ~CDX12Device();

	//Lion
	//protected:
		bool initDevice(HWND hWnd);

	private:
		void GetHardwareAdapter(_In_ IDXGIFactory1* pFactory, _Outptr_result_maybenull_ IDXGIAdapter1** ppAdapter,
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