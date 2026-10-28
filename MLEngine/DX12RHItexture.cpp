#include "DX12RHItexture.h"
#include "GraphicPlug.h"
#include "MLEngine.h"
#include "DX12Device.h"

namespace ML
{
	CDX12RHITexture2D::~CDX12RHITexture2D()
	{
		Release();
	}

	bool CDX12RHITexture2D::Create(DXGI_FORMAT InFormat, uint32 InWidth, uint32 InHeight,
		const void* InData, uint32 InDataRowPitch)
	{
		Release();

		if (nullptr == InData || 0 == InWidth || 0 == InHeight || DXGI_FORMAT_UNKNOWN == InFormat)
		{
			return false;
		}

		CGraphicPlug* pPlug = GEngine ? GEngine->GetPlugs<CGraphicPlug>() : nullptr;
		CDX12RHIDevice* pRHIDevice = pPlug ? pPlug->GetDevice() : nullptr;
		ID3D12Device* pDevice = pRHIDevice ? pRHIDevice->GetDevice() : nullptr;
		if (nullptr == pDevice)
		{
			OutputDebugStringA("CDX12RHITexture2D::Create: RHI device is not ready\n");
			return false;
		}

		Format = InFormat;
		Width = InWidth;
		Height = InHeight;

		// 1) Destination lives in DEFAULT (GPU-only) memory.
		D3D12_RESOURCE_DESC TexDesc = {};
		TexDesc.Dimension          = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		TexDesc.Alignment          = 0;
		TexDesc.Width              = Width;
		TexDesc.Height             = Height;
		TexDesc.DepthOrArraySize   = 1;
		TexDesc.MipLevels          = 1;   // P0: no mip chain, no streaming yet
		TexDesc.Format             = Format;
		TexDesc.SampleDesc.Count   = 1;
		TexDesc.SampleDesc.Quality = 0;
		TexDesc.Layout             = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		TexDesc.Flags              = D3D12_RESOURCE_FLAG_NONE;

		D3D12_HEAP_PROPERTIES DefaultHeapProps = {};
		DefaultHeapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

		if (FAILED(pDevice->CreateCommittedResource(&DefaultHeapProps, D3D12_HEAP_FLAG_NONE, &TexDesc,
			D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&Resource))))
		{
			Release();
			return false;
		}

		// 2) Staging buffer sized by the driver's copyable footprint: the row pitch
		//    inside an upload heap must be 256-byte aligned, so the tight pitch
		//    coming from the decoder cannot be used directly.
		D3D12_SUBRESOURCE_DATA SrcData = {};
		SrcData.pData      = InData;
		SrcData.RowPitch   = InDataRowPitch;
		SrcData.SlicePitch = (UINT64)InDataRowPitch * Height;

		const UINT64 UploadSize = GetRequiredIntermediateSize(Resource, 0, 1);

		D3D12_HEAP_PROPERTIES UploadHeapProps = {};
		UploadHeapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

		D3D12_RESOURCE_DESC UploadDesc = {};
		UploadDesc.Dimension          = D3D12_RESOURCE_DIMENSION_BUFFER;
		UploadDesc.Alignment          = 0;
		UploadDesc.Width              = UploadSize;
		UploadDesc.Height             = 1;
		UploadDesc.DepthOrArraySize   = 1;
		UploadDesc.MipLevels          = 1;
		UploadDesc.Format             = DXGI_FORMAT_UNKNOWN;
		UploadDesc.SampleDesc.Count   = 1;
		UploadDesc.SampleDesc.Quality = 0;
		UploadDesc.Layout             = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		UploadDesc.Flags              = D3D12_RESOURCE_FLAG_NONE;

		ID3D12Resource* pUploadBuffer = nullptr;
		if (FAILED(pDevice->CreateCommittedResource(&UploadHeapProps, D3D12_HEAP_FLAG_NONE, &UploadDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&pUploadBuffer))))
		{
			Release();
			return false;
		}

		// 3) Record the copy on a throwaway list, exactly like CDX12RHIBuffer does:
		//    the main command list belongs to the render thread's frame loop.
		ID3D12CommandAllocator* pUploadAllocator = nullptr;
		ID3D12GraphicsCommandList* pUploadList = nullptr;
		if (FAILED(pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&pUploadAllocator))) ||
			FAILED(pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, pUploadAllocator, nullptr, IID_PPV_ARGS(&pUploadList))))
		{
			SafeRelease(pUploadAllocator);
			SafeRelease(pUploadBuffer);
			Release();
			return false;
		}

		// Copies row-by-row respecting the upload heap's 256B row alignment.
		// The explicit <1> picks d3dx12's stack-allocating overload (MaxSubresources
		// = 1 mip-less texture), which is why the subresource count is not passed here.
		UpdateSubresources<1>(pUploadList, Resource, pUploadBuffer, 0, 0, 1, &SrcData);

		D3D12_RESOURCE_BARRIER Barrier = {};
		Barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		Barrier.Transition.pResource   = Resource;
		Barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		Barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		Barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		pUploadList->ResourceBarrier(1, &Barrier);

		// Blocks until the GPU finished the copy, then releases pUploadList.
		pRHIDevice->ExecuteUploadAndWait(pUploadList);
		pUploadAllocator->Release();
		SafeRelease(pUploadBuffer);

		// 4) Shader resource view: one descriptor in the device's shader-visible heap.
		D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
		SRVDesc.Format                     = Format;
		SRVDesc.ViewDimension              = D3D12_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Shader4ComponentMapping    = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		SRVDesc.Texture2D.MipLevels        = 1;
		SRVDesc.Texture2D.MostDetailedMip  = 0;
		SRVDesc.Texture2D.PlaneSlice       = 0;
		SRVDesc.Texture2D.ResourceMinLODClamp = 0.0f;

		UINT32 Slot = 0;
		if (!pRHIDevice->AllocSRV(Resource, SRVDesc, Slot))
		{
			OutputDebugStringA("CDX12RHITexture2D::Create: SRV descriptor heap exhausted\n");
			Release();
			return false;
		}
		SRVSlot = Slot;
		bHasSRV = true;
		return true;
	}

	void CDX12RHITexture2D::Release()
	{
		CGraphicPlug* pPlug = GEngine ? GEngine->GetPlugs<CGraphicPlug>() : nullptr;
		CDX12RHIDevice* pRHIDevice = pPlug ? pPlug->GetDevice() : nullptr;
		if (pRHIDevice && bHasSRV)
		{
			pRHIDevice->FreeSRV(SRVSlot);
		}
		bHasSRV = false;
		SRVSlot = 0;

		SafeRelease(Resource);
		Format = DXGI_FORMAT_UNKNOWN;
		Width = 0;
		Height = 0;
	}

	D3D12_GPU_DESCRIPTOR_HANDLE CDX12RHITexture2D::GetSRVGPUHandle() const
	{
		D3D12_GPU_DESCRIPTOR_HANDLE Handle = {};
		if (!bHasSRV)
		{
			return Handle;
		}

		CGraphicPlug* pPlug = GEngine ? GEngine->GetPlugs<CGraphicPlug>() : nullptr;
		CDX12RHIDevice* pRHIDevice = pPlug ? pPlug->GetDevice() : nullptr;
		return pRHIDevice ? pRHIDevice->GetSRVGPUHandle(SRVSlot) : Handle;
	}
}
