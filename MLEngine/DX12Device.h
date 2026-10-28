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

		// Capacity of the shader-visible CBV/SRV/UAV heap. One slot per texture
		// (mip-less P0 textures), so this is also how many textures can be
		// resident at once. UE counterpart: the online descriptor allocator's
		// descriptor blocks - unbounded there, fixed here until we need the
		// same rolling/frame-descriptor machinery.
		static constexpr UINT32 MaxSRVDescriptors = 512;

		CDX12RHIDevice();
		virtual ~CDX12RHIDevice();
		void BeginDraw();
		void EndDraw();
		ID3D12Device* GetDevice() const;
		ID3D12GraphicsCommandList* GetCommandList() const;
		void ExecuteUploadAndWait(ID3D12GraphicsCommandList* UploadList);
		bool initDevice(HWND hWnd);

		// -------------------------------------------------------------------
		// Shader-visible descriptors.
		//
		// A texture reaches the pixel shader as a GPU handle into these heaps,
		// not as a resource pointer - which is why the device owns them: every
		// SetGraphicsRootDescriptorTable in a frame indexes the same heap pair.
		// UE solves this with FD3D12DescriptorCache + its online descriptor
		// manager; P0 keeps the whole thing here as a free-list over one heap.
		// -------------------------------------------------------------------

		// Allocates one descriptor in the CBV/SRV/UAV heap and writes the view.
		// Returns false once the heap is full. Thread-safe: level loading may
		// upload textures from a worker thread while the renderer draws.
		bool AllocSRV(ID3D12Resource* InResource, const D3D12_SHADER_RESOURCE_VIEW_DESC& Desc, UINT32& OutSlot);
		void FreeSRV(UINT32 Slot);

		D3D12_CPU_DESCRIPTOR_HANDLE GetSRVCPUHandle(UINT32 Slot) const;
		D3D12_GPU_DESCRIPTOR_HANDLE GetSRVGPUHandle(UINT32 Slot) const;

		// The heaps themselves, for SetDescriptorHeaps before a draw.
		ID3D12DescriptorHeap* GetSrvHeap() const { return mSrvHeap; }
		ID3D12DescriptorHeap* GetSamplerHeap() const { return mSamplerHeap; }

		// The engine's single static sampler: linear filtering, wrap addressing.
		// D3D12 cannot place samplers in a root signature by value the way UE's
		// static sampler states do, so one immutable sampler serves every
		// base-color fetch until per-material sampler states arrive.
		D3D12_GPU_DESCRIPTOR_HANDLE GetStaticSamplerGPUHandle() const;

	private:
		void _GetHardwareAdapter(_In_ IDXGIFactory1* pFactory, _Outptr_result_maybenull_ IDXGIAdapter1** ppAdapter, bool requestHighPerformanceAdapter = true);
		void _WaitCommandQueue();
		bool _InitDepthBuffer();
		bool _InitShaderVisibleHeaps();

	private:

		UINT32 mRtvDescriptorSize = 0;
		ID3D12Device* mpDevice = nullptr;
		ID3D12CommandQueue* mCommandQueue = nullptr;
		IDXGISwapChain3* mSwapChain = nullptr;
		ID3D12DescriptorHeap* mRtvHeap = nullptr;
		// Depth buffer for 3D rendering (D32).
		ID3D12DescriptorHeap* mDsvHeap = nullptr;
		ID3D12Resource* mDepthStencil = nullptr;

		// Shader-visible CBV/SRV/UAV heap + the free list that hands out slots.
		ID3D12DescriptorHeap* mSrvHeap = nullptr;
		UINT32 mCbvSrvDescriptorSize = 0;
		UINT32 mNextSRVSlot = 0;
		std::vector<UINT32> mFreeSRVSlots;
		std::mutex mSRVMutex;

		// Shader-visible sampler heap holding the one static sampler.
		ID3D12DescriptorHeap* mSamplerHeap = nullptr;
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
