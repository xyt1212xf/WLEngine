#pragma once
#include "Refcount.h"
#include "Device.h"
#include <d3d12.h>
#include <vector>

namespace ML
{
	struct FTextureRHIInfo
	{
		DXGI_FORMAT Format = DXGI_FORMAT_UNKNOWN;
		uint32 Width = 0;
		uint32 Height = 0;
		uint16 ArraySize = 1;
		uint16 MipLevels = 1;
		bool   bIsCubemap = false;
	};

	// UE: FD3D12TextureBase / FD3D12Texture2D - the RHI-side counterpart of a
	// texture object. UE owns this on the render thread (created by
	// UTexture::UpdateResource -> FTexture2DResource::InitRHI); P0 lets the asset
	// own it, the same shortcut CMesh already takes for its vertex buffers.
	//
	// The SRV descriptor is allocated out of the device's shader-visible heap and
	// returned to its free list in Release(). UE does this through its online
	// descriptor manager; here CDX12RHIDevice owns the heap and hands out slots.
	class CDX12RHITexture2D : public CRefcount
	{
	public:
		CDX12RHITexture2D() = default;
		virtual ~CDX12RHITexture2D();

		// Creates the DEFAULT-heap texture, copies InData in, and allocates its SRV.
		// InData is RGBA8 row-major; InDataRowPitch may be the tight pitch
		// (Width * 4) - UpdateSubresources handles the driver's row alignment.
		// Returns whether the texture is ready for pixel-shader access.
		virtual bool Create(const FTextureRHIInfo& Info, const void* InData, uint32 InDataRowPitch);
		virtual bool Create(const FTextureRHIInfo& Info, D3D12_SUBRESOURCE_DATA* InSlices, uint32 NumSlices);
		virtual void Release();

		bool IsValid() const { return nullptr != Resource; }
		uint32 GetWidth()  const { return Width; }
		uint32 GetHeight() const { return Height; }
		DXGI_FORMAT GetFormat() const { return Format; }

		// GPU handle the renderer feeds to SetGraphicsRootDescriptorTable.
		// Empty handle when the texture has no SRV (not created / already released).
		D3D12_GPU_DESCRIPTOR_HANDLE GetSRVGPUHandle() const;

	protected:
		// Shared implementation of both public Create overloads. bArrayView
		// selects the SRV dimension explicitly instead of inferring it from
		// ArraySize: a one-slice array must still be a TEXTURE2DARRAY view
		// (the shader declares Texture2DArray), while a plain 2D texture must
		// stay TEXTURE2D.
		bool CreateInternal(const FTextureRHIInfo& Info, D3D12_SUBRESOURCE_DATA* InSlices, uint32 NumSlices, bool bArrayView);

		ID3D12Resource* Resource = nullptr;
		DXGI_FORMAT     Format = DXGI_FORMAT_UNKNOWN;
		uint32          Width = 0;
		uint32          Height = 0;
		uint16          ArraySize = 1;
		uint16          MipLevels = 1;
		bool            bIsCubemap = false;

		// Descriptor bookkeeping: slot index in the device heap, and whether it
		// was allocated at all (0 is a valid slot).
		UINT32 SRVSlot = 0;
		bool   bHasSRV = false;
	};
}
