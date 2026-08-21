#include "DX12Device.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

namespace WL
{
    static const UINT32 FrameCount = 2;
	CDX12Device::CDX12Device()
	{

	}

	CDX12Device::~CDX12Device()
	{
	}

	bool CDX12Device::initDevice(HWND hWnd)
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
				GetHardwareAdapter(factory, &hardwareAdapter);
				for (auto level : levels)
				{
					if (SUCCEEDED(D3D12CreateDevice(hardwareAdapter, level,IID_PPV_ARGS(&mpDevice))))
					{
						// 成功创建，device 支持该 Feature Level
						break;
					}
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
				//Lion
				swapChainDesc.Width = 1024;
				swapChainDesc.Height = 768;

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
				hr = mpDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&mCommandAllocator));
				if (hr != S_OK)
				{
					break;
				}
			}
			// Create the command list.
			mpDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mCommandAllocator, nullptr, IID_PPV_ARGS(&mCommandList));

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
			}
			return true;
		} while (false);
		return false;
	}

	void CDX12Device::GetHardwareAdapter(_In_ IDXGIFactory1* pFactory, _Outptr_result_maybenull_ IDXGIAdapter1** ppAdapter,
		bool requestHighPerformanceAdapter /*= false*/)
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
}