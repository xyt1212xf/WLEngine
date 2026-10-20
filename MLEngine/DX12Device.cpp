#include "DX12Device.h"
#include "MLEngine.h"

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3dcompiler.lib")

namespace ML
{
	CDX12RHIDevice::CDX12RHIDevice()
	: mFenceValues(FrameCount, 0)
	{
	}

	CDX12RHIDevice::~CDX12RHIDevice()
	{
		// Tear down in reverse creation order; every ID3D12* must go before the device.
		SafeRelease(mDepthStencil);
		SafeRelease(mDsvHeap);
		SafeRelease(mCommandList);
		for (ID3D12CommandAllocator*& Allocator : mCommandAllocators)
		{
			SafeRelease(Allocator);
		}
		mCommandAllocators.clear();
		for (ID3D12Resource*& RenderTarget : mRenderTargets)
		{
			SafeRelease(RenderTarget);
		}
		mRenderTargets.clear();
		SafeRelease(mRtvHeap);
		SafeRelease(mSwapChain);
		SafeRelease(mCommandQueue);

		if (mFenceEvent != nullptr)
		{
			CloseHandle(mFenceEvent);
			mFenceEvent = nullptr;
		}
		SafeRelease(mFence);

		if (mUploadFenceEvent != nullptr)
		{
			CloseHandle(mUploadFenceEvent);
			mUploadFenceEvent = nullptr;
		}
		SafeRelease(mUploadFence);

		SafeRelease(mpDevice);
	}

	bool CDX12RHIDevice::initDevice(HWND hWnd)
	{
		do 
		{
			UINT32 dxgiFactoryFlags = 0;
	#if defined(_DEBUG)
			// Enable the debug layer (requires the Graphics Tools "optional feature").
			// NOTE: Enabling the debug layer after device creation will invalidate the active device.
			{
				ID3D12Debug* debugController = nullptr;
				if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
				{
					debugController->EnableDebugLayer();

					// Enable additional debug layers.
					dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
				}
			}
	#endif	
			const SWindowConfig* winConfig = GEngine->GetPlatform().getWindowConfigPtr();
			IDXGIFactory4* factory = nullptr;
			HRESULT hr = CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory));
			if (hr == S_OK)
			{
				D3D_FEATURE_LEVEL levels[] =
				{
					D3D_FEATURE_LEVEL_12_1,
					D3D_FEATURE_LEVEL_12_0,
					D3D_FEATURE_LEVEL_11_1,
					D3D_FEATURE_LEVEL_11_0
				};
				IDXGIAdapter1* hardwareAdapter = nullptr;
				_GetHardwareAdapter(factory, &hardwareAdapter);
				for (auto level : levels)
				{
					if (SUCCEEDED(D3D12CreateDevice(hardwareAdapter, level,IID_PPV_ARGS(&mpDevice))))
					{
						// 成功创建，device 支持该 Feature Level
						break;
					}
				}
				SafeRelease(hardwareAdapter);
				if (nullptr == mpDevice)
				{
					break;
				}

				// Describe and create the command queue.
				D3D12_COMMAND_QUEUE_DESC queueDesc = {};
				queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
				queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

				hr = mpDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mCommandQueue));
				if (hr != S_OK)
				{	
					break;
				}
				// Describe and create the swap chain.
				DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
				swapChainDesc.BufferCount = FrameCount;
				swapChainDesc.Width = winConfig->nWidth;
				swapChainDesc.Height = winConfig->nHeight;

				swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
				swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
				swapChainDesc.SampleDesc.Count = 1;

				IDXGISwapChain1* swapChain = nullptr;
				hr = factory->CreateSwapChainForHwnd(
					mCommandQueue,        // Swap chain needs the queue so that it can force a flush on it.
					hWnd,
					&swapChainDesc,
					nullptr,
					nullptr,
					&swapChain);
				if (hr != S_OK)
				{
					break;
				}
				// This sample does not support fullscreen transitions.
				factory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);
				factory->Release();

				hr = swapChain->QueryInterface(IID_PPV_ARGS(&mSwapChain));
				swapChain->Release();
				mFrameIndex = mSwapChain->GetCurrentBackBufferIndex();

				// Create descriptor heaps.
				{
					// Describe and create a render target view (RTV) descriptor heap.
					D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
					rtvHeapDesc.NumDescriptors = FrameCount;
					rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
					rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
					hr = mpDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&mRtvHeap));
					if (hr != S_OK)
					{
						break;
					}

					mRtvDescriptorSize = mpDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
				}
				// Create frame resources.
				{
					CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(mRtvHeap->GetCPUDescriptorHandleForHeapStart());

					// Create a RTV for each frame.
					mRenderTargets.resize(FrameCount);
					for (UINT n = 0; n < FrameCount; n++)
					{
						mSwapChain->GetBuffer(n, IID_PPV_ARGS(&mRenderTargets[n]));
						mpDevice->CreateRenderTargetView(mRenderTargets[n], nullptr, rtvHandle);
						rtvHandle.Offset(1, mRtvDescriptorSize);
					}
				}
				// One allocator per frame in flight (see mCommandAllocators in the header).
				mCommandAllocators.assign(FrameCount, nullptr);
				for (UINT32 i = 0; i < FrameCount; ++i)
				{
					if (FAILED(mpDevice->CreateCommandAllocator(
						D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&mCommandAllocators[i]))))
					{
						// Any single failure aborts device creation: BeginDraw would
						// otherwise Reset() a nullptr allocator.
						return false;
					}
				}

				// Depth buffer (needed by the 3D pipeline PSO).
				if (!_InitDepthBuffer())
				{
					break;
				}
			}
			// Everything below assumes a live device and a full allocator set. If the
			// factory block above bailed out early, mpDevice is still null - fail cleanly
			// instead of dereferencing it.
			if (nullptr == mpDevice || mCommandAllocators.size() != FrameCount)
			{
				break;
			}

			// Create the command list, bound to the current frame's allocator.
			if (FAILED(mpDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
				mCommandAllocators[mFrameIndex], nullptr, IID_PPV_ARGS(&mCommandList))))
			{
				break;
			}

			// Command lists are created in the recording state, but there is nothing
			// to record yet. The main loop expects it to be closed, so close it now.
			mCommandList->Close();
			// Create synchronization objects.
			{
				mpDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence));
				mFenceValue = 1;

				// Create an event handle to use for frame synchronization.
				mFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
				if (mFenceEvent == nullptr)
				{
					HRESULT_FROM_WIN32(GetLastError());
					break;
				}

				// Upload path gets its own fence + event (see header comment).
				if (FAILED(mpDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mUploadFence))))
				{
					CloseHandle(mFenceEvent);
					mFenceEvent = nullptr;
					break;
				}
				mUploadFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
				if (mUploadFenceEvent == nullptr)
				{
					HRESULT_FROM_WIN32(GetLastError());
					CloseHandle(mFenceEvent);
					mFenceEvent = nullptr;
					break;
				}
			}

			mScreenViewport.TopLeftX = 0;
			mScreenViewport.TopLeftY = 0;
			mScreenViewport.Width = static_cast<float>(winConfig->nWidth);
			mScreenViewport.Height = static_cast<float>(winConfig->nHeight);
			mScreenViewport.MinDepth = 0.0f;
			mScreenViewport.MaxDepth = 1.0f;

			mScissorRect = { 0, 0, winConfig->nWidth, winConfig->nHeight};

			return true;
		} while (false);
		return false;
	}

	void CDX12RHIDevice::_GetHardwareAdapter(_In_ IDXGIFactory1* pFactory, _Outptr_result_maybenull_ IDXGIAdapter1** ppAdapter,
		bool requestHighPerformanceAdapter /*= true*/)
	{
		*ppAdapter = nullptr;
		IDXGIAdapter1* adapter = nullptr;
		IDXGIFactory6* factory6 = nullptr;
		if (SUCCEEDED(pFactory->QueryInterface(IID_PPV_ARGS(&factory6))))
		{
			for (UINT adapterIndex = 0;
				   SUCCEEDED(factory6->EnumAdapterByGpuPreference(
					adapterIndex,
					requestHighPerformanceAdapter == true ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE : DXGI_GPU_PREFERENCE_UNSPECIFIED,
					IID_PPV_ARGS(&adapter)));
					++adapterIndex)
			{
				DXGI_ADAPTER_DESC1 desc;
				adapter->GetDesc1(&desc);

				if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
				{
					// Don't select the Basic Render Driver adapter.
					// If you want a software adapter, pass in "/warp" on the command line.
					continue;
				}

				// Check to see whether the adapter supports Direct3D 12, but don't create the
				// actual device yet.
				if (SUCCEEDED(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
				{
					SafeRelease(factory6);
					break;
				}
			}
		}

		if (adapter == nullptr)
		{
			for (UINT adapterIndex = 0; SUCCEEDED(pFactory->EnumAdapters1(adapterIndex, &adapter)); ++adapterIndex)
			{
				DXGI_ADAPTER_DESC1 desc;
				adapter->GetDesc1(&desc);

				if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
				{
					// Don't select the Basic Render Driver adapter.
					// If you want a software adapter, pass in "/warp" on the command line.
					continue;
				}

				// Check to see whether the adapter supports Direct3D 12, but don't create the
				// actual device yet.
				if (SUCCEEDED(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
				{
					break;
				}
			}
		}
		*ppAdapter = adapter;
	}


	void CDX12RHIDevice::BeginDraw()
	{
		// Flip-model: ask the swap chain which back buffer is ours this frame
		// instead of tracking the index by hand.
		mFrameIndex = mSwapChain->GetCurrentBackBufferIndex();

		// Wait only for the frame slot we are about to overwrite - NOT for the
		// previous frame. With one allocator per frame the GPU may stay a frame
		// behind while the CPU records ahead, so this rarely blocks.
		_WaitCommandQueue();

		// Reuse the memory associated with command recording.
		// We can only reset when the associated command lists have finished execution on the GPU.
		ID3D12CommandAllocator* Allocator = mCommandAllocators[mFrameIndex];
		Allocator->Reset();

		// A command list can be reset after it has been added to the command queue via ExecuteCommandList.
		// Reusing the command list reuses memory.
		mCommandList->Reset(Allocator, nullptr);

		CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			mRenderTargets[mFrameIndex],
			D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_RENDER_TARGET);

		// Indicate a state transition on the resource usage.
		mCommandList->ResourceBarrier(1, &barrier);

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtv(
			mRtvHeap->GetCPUDescriptorHandleForHeapStart(),
			mFrameIndex,
			mRtvDescriptorSize);

		// Bind RTV + DSV, clear both.
		CD3DX12_CPU_DESCRIPTOR_HANDLE dsv(mDsvHeap->GetCPUDescriptorHandleForHeapStart());
		mCommandList->OMSetRenderTargets(1, &rtv, FALSE, &dsv);
		static const float clearColor[] = { 0.0f, 0.0f, 1.0f, 1.0f };
		mCommandList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);
		mCommandList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);


		// Set the viewport and scissor rect.  This needs to be reset whenever the command list is reset.
		mCommandList->RSSetViewports(1, &mScreenViewport);
		mCommandList->RSSetScissorRects(1, &mScissorRect);

	}

	void CDX12RHIDevice::EndDraw()
	{
		CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			mRenderTargets[mFrameIndex],
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT);
		mCommandList->ResourceBarrier(1, &barrier);

		mCommandList->Close();
		// Add the command list to the queue for execution.
		ID3D12CommandList* cmdsLists[] = { mCommandList };
		mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

		// swap the back and front buffers
		mSwapChain->Present(0, 0);

		// Stamp this frame slot with the fence value the GPU will reach when it is
		// done. BeginDraw waits on it when this slot comes round again.
		const UINT64 FenceForThisFrame = mFenceValue++;
		mCommandQueue->Signal(mFence, FenceForThisFrame);
		mFenceValues[mFrameIndex] = FenceForThisFrame;
	}


	ID3D12Device* CDX12RHIDevice::GetDevice() const
	{
		return mpDevice;
	}

	ID3D12GraphicsCommandList* CDX12RHIDevice::GetCommandList() const
	{
		return mCommandList;
	}

	void CDX12RHIDevice::ExecuteUploadAndWait(ID3D12GraphicsCommandList* UploadList)
	{
		UploadList->Close();
		ID3D12CommandList* lists[] = { UploadList };
		mCommandQueue->ExecuteCommandLists(1, lists);   // the queue is free-threaded

		// Own fence + own event: several loading threads may upload at once, and the
		// render thread is signalling the frame fence on the same queue concurrently.
		const UINT64 FenceToWait = mUploadFenceValue.fetch_add(1) + 1;
		mCommandQueue->Signal(mUploadFence, FenceToWait);
		mUploadFence->SetEventOnCompletion(FenceToWait, mUploadFenceEvent);  // CPU 阻塞等拷贝完成
		WaitForSingleObject(mUploadFenceEvent, INFINITE);

		UploadList->Release();      // 用完即弃（或做成成员复用）
	}

	bool CDX12RHIDevice::_InitDepthBuffer()
	{
		const SWindowConfig* winConfig = GEngine->GetPlatform().getWindowConfigPtr();

		// DSV descriptor heap (single depth target).
		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
		dsvHeapDesc.NumDescriptors = 1;
		dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		if (FAILED(mpDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&mDsvHeap))))
		{
			return false;
		}

		D3D12_CLEAR_VALUE clearValue = {};
		clearValue.Format = DXGI_FORMAT_D32_FLOAT;
		clearValue.DepthStencil.Depth = 1.0f;

		D3D12_HEAP_PROPERTIES heapProps = {};
		heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

		D3D12_RESOURCE_DESC depthDesc = {};
		depthDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		depthDesc.Alignment = 0;
		depthDesc.Width = winConfig->nWidth;
		depthDesc.Height = winConfig->nHeight;
		depthDesc.DepthOrArraySize = 1;
		depthDesc.MipLevels = 1;
		depthDesc.Format = DXGI_FORMAT_D32_FLOAT;
		depthDesc.SampleDesc.Count = 1;
		depthDesc.SampleDesc.Quality = 0;
		depthDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		depthDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		HRESULT hr = mpDevice->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&depthDesc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&clearValue,
			IID_PPV_ARGS(&mDepthStencil));
		if (FAILED(hr))
		{
			return false;
		}

		mpDevice->CreateDepthStencilView(mDepthStencil, nullptr, mDsvHeap->GetCPUDescriptorHandleForHeapStart());
		return true;
	}

	void CDX12RHIDevice::_WaitCommandQueue()
	{
		// Wait for the frame slot we are about to record into (not the previous frame).
		// No hardcoded & 0x01 - works for any FrameCount.
		const UINT64 fenceToWait = mFenceValues[mFrameIndex];

		if (fenceToWait > 0 && mFence->GetCompletedValue() < fenceToWait)
		{
			mFence->SetEventOnCompletion(fenceToWait, mFenceEvent);
			WaitForSingleObject(mFenceEvent, INFINITE);
		}
	}

}
