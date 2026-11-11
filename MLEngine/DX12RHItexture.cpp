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

	bool CDX12RHITexture2D::Create(const FTextureRHIInfo& Info, const void* InData, uint32 InDataRowPitch)
	{
		if (nullptr == InData || 0 == Info.Width || 0 == Info.Height || DXGI_FORMAT_UNKNOWN == Info.Format)
		{
			return false;
		}
		// Single 2D texture = one subresource, tight row pitch, SlicePitch covers
		// the whole image. bArrayView = false: the SRV stays TEXTURE2D so
		// `Texture2D.Sample` in the shaders keeps matching the descriptor.
		D3D12_SUBRESOURCE_DATA Slice = {};
		Slice.pData = InData;
		Slice.RowPitch = InDataRowPitch;
		Slice.SlicePitch = (UINT64)InDataRowPitch * Info.Height;
		return CreateInternal(Info, &Slice, 1, false);
	}

	bool CDX12RHITexture2D::Create(const FTextureRHIInfo& Info, D3D12_SUBRESOURCE_DATA* InSlices, uint32 NumSlices)
	{
		// bArrayView = true even for NumSlices == 1: the caller declared a
		// Texture2DArray in HLSL, and the D3D12 runtime does not validate SRV
		// dimension against the shader's type - a TEXTURE2D view there is
		// undefined behaviour (garbage samples, no debug error).
		return CreateInternal(Info, InSlices, NumSlices, true);
	}

	bool CDX12RHITexture2D::CreateInternal(const FTextureRHIInfo& Info, D3D12_SUBRESOURCE_DATA* InSlices, uint32 NumSlices, bool bArrayView)
	{
		Release();

		if (nullptr == InSlices || 0 == NumSlices || 0 == Info.Width || 0 == Info.Height || DXGI_FORMAT_UNKNOWN == Info.Format)
		{
			return false;
		}
		// UpdateSubresources' template parameter is its stack scratch size; the
		// layer blend uses at most 4 slices, keep headroom without heap traffic.
		if (NumSlices > 16)
		{
			OutputDebugStringA("CDX12RHITexture2D::Create: NumSlices exceeds the 16-subresource upload path\n");
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

		Format = Info.Format;
		Width = Info.Width;
		Height = Info.Height;
		ArraySize = Info.ArraySize;

		// 1) Destination lives in DEFAULT (GPU-only) memory.
		D3D12_RESOURCE_DESC TexDesc = {};
		TexDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		TexDesc.Alignment = 0;
		TexDesc.Width = Width;
		TexDesc.Height = Height;
		TexDesc.DepthOrArraySize = ArraySize;
		TexDesc.MipLevels = 1;   // P0: no mip chain, no streaming yet
		TexDesc.Format = Format;
		TexDesc.SampleDesc.Count = 1;
		TexDesc.SampleDesc.Quality = 0;
		TexDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		TexDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

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
		//    coming from the decoder cannot be used directly. For an array the
		//    subresources are the individual slices (slice * MipLevels ordering).
		const UINT NumSubresources = (UINT)ArraySize;
		const UINT64 UploadSize = GetRequiredIntermediateSize(Resource, 0, NumSubresources);

		D3D12_HEAP_PROPERTIES UploadHeapProps = {};
		UploadHeapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

		D3D12_RESOURCE_DESC UploadDesc = {};
		UploadDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		UploadDesc.Alignment = 0;
		UploadDesc.Width = UploadSize;
		UploadDesc.Height = 1;
		UploadDesc.DepthOrArraySize = 1;
		UploadDesc.MipLevels = 1;
		UploadDesc.Format = DXGI_FORMAT_UNKNOWN;
		UploadDesc.SampleDesc.Count = 1;
		UploadDesc.SampleDesc.Quality = 0;
		UploadDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		UploadDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

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

		// Copies slice-by-slice respecting the upload heap's 256B row alignment.
		UpdateSubresources<16>(pUploadList, Resource, pUploadBuffer, 0, 0, NumSlices, InSlices);

		D3D12_RESOURCE_BARRIER Barrier = {};
		Barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		Barrier.Transition.pResource = Resource;
		Barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		Barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		Barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		pUploadList->ResourceBarrier(1, &Barrier);

		// Blocks until the GPU finished the copy, then releases pUploadList.
		pRHIDevice->ExecuteUploadAndWait(pUploadList);
		pUploadAllocator->Release();
		SafeRelease(pUploadBuffer);

		// 4) Shader resource view: one descriptor in the device's shader-visible heap.
		//    Collection arrays expose TEXTURE2DARRAY (HLSL: Texture2DArray.Sample(float3(uv, slice)))
		//    even when they hold a single slice; the view dimension must match the
		//    shader's declared type, not the slice count.
		D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
		SRVDesc.Format = Format;
		SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		if (bArrayView)
		{
			SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
			SRVDesc.Texture2DArray.MipLevels = 1;
			SRVDesc.Texture2DArray.MostDetailedMip = 0;
			SRVDesc.Texture2DArray.FirstArraySlice = 0;
			SRVDesc.Texture2DArray.ArraySize = ArraySize;
			SRVDesc.Texture2DArray.ResourceMinLODClamp = 0.0f;
		}
		else
		{
			SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			SRVDesc.Texture2D.MipLevels = 1;
			SRVDesc.Texture2D.MostDetailedMip = 0;
			SRVDesc.Texture2D.PlaneSlice = 0;
			SRVDesc.Texture2D.ResourceMinLODClamp = 0.0f;
		}

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
		ArraySize = 1;
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
