#pragma once 
#include <d3d12.h>
#include "d3dx12.h"
#include <dxgi1_6.h>
#include "Common.h"
#include "Refcount.h"

namespace ML
{
	class CDX12RHIDevice : public CRefcount
	{
		friend class CGraphicPlug;
	public:
		// Number of back buffers == number of frames that may be in flight.
		static constexpr UINT32 FrameCount = 2;

		CDX12RHIDevice();
		virtual ~CDX12RHIDevice();
		void BeginDraw();
		void EndDraw();
		ID3D12Device* GetDevice() const;
		ID3D12GraphicsCommandList* GetCommandList() const;
		void ExecuteUploadAndWait(ID3D12GraphicsCommandList* UploadList);
		bool initDevice(HWND hWnd);

	private:
		void _GetHardwareAdapter(_In_ IDXGIFactory1* pFactory, _Outptr_result_maybenull_ IDXGIAdapter1** ppAdapter, bool requestHighPerformanceAdapter = true);
		void _WaitCommandQueue();
		bool _InitDepthBuffer();

	private:

		UINT32 mRtvDescriptorSize = 0;
		ID3D12Device* mpDevice = nullptr;
		ID3D12CommandQueue* mCommandQueue = nullptr;
		IDXGISwapChain3* mSwapChain = nullptr;
		ID3D12DescriptorHeap* mRtvHeap = nullptr;
		// Depth buffer for 3D rendering (D32).
		ID3D12DescriptorHeap* mDsvHeap = nullptr;
		ID3D12Resource* mDepthStencil = nullptr;
		// One command allocator per frame in flight: an allocator may only be Reset
		// once the GPU finished executing everything recorded with it. With a single
		// allocator the CPU is forced to wait for the previous frame; one per frame
		// lets the CPU record ahead while the GPU is still busy.
		std::vector<ID3D12CommandAllocator*> mCommandAllocators;
		ID3D12GraphicsCommandList* mCommandList = nullptr;
		std::vector<ID3D12Resource*> mRenderTargets;

		D3D12_VIEWPORT mScreenViewport;
		D3D12_RECT mScissorRect;

		// Synchronization objects (frame loop, render thread).
		std::vector<UINT64> mFenceValues; 
		UINT32 mFrameIndex = 0;
		HANDLE mFenceEvent = nullptr;
		ID3D12Fence* mFence = nullptr;
		UINT64 mFenceValue = 0;

		// Synchronization objects (resource upload path).
		// Must be separate from the frame-loop fence: uploads are issued from the
		// world-loading thread while the render thread signals the frame fence on
		// the same queue. Sharing one event handle lets a waiter wake on the wrong
		// thread's signal.
		HANDLE mUploadFenceEvent = nullptr;
		ID3D12Fence* mUploadFence = nullptr;
		std::atomic<UINT64> mUploadFenceValue{ 0 };
	};
}
