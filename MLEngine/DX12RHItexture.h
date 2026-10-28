#pragma once
#include "Refcount.h"
#include "Device.h"

namespace ML
{
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
		virtual bool Create(DXGI_FORMAT InFormat, uint32 InWidth, uint32 InHeight,
			const void* InData, uint32 InDataRowPitch);
		virtual void Release();

		bool IsValid() const { return nullptr != Resource; }
		uint32 GetWidth()  const { return Width; }
		uint32 GetHeight() const { return Height; }
		DXGI_FORMAT GetFormat() const { return Format; }

		// GPU handle the renderer feeds to SetGraphicsRootDescriptorTable.
		// Empty handle when the texture has no SRV (not created / already released).
		D3D12_GPU_DESCRIPTOR_HANDLE GetSRVGPUHandle() const;

	protected:
		ID3D12Resource* Resource = nullptr;
		DXGI_FORMAT     Format = DXGI_FORMAT_UNKNOWN;
		uint32          Width = 0;
		uint32          Height = 0;

		// Descriptor bookkeeping: slot index in the device heap, and whether it
		// was allocated at all (0 is a valid slot).
		UINT32 SRVSlot = 0;
		bool   bHasSRV = false;
	};
}
