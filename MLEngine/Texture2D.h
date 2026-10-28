#pragma once
#include "StreamableRenderAsset.h"
#include <string>
#include <vector>

namespace ML
{
	class CRHITexture2D;

	//====================================================================
	// UE: UTexture2D : UTexture : UStreamableRenderAsset : UObject
	//
	// The second - and until now missing - subclass of the streaming facet.
	// Textures share it with meshes for exactly the same reason: their GPU
	// payload can be dropped and streamed back in (mip levels <=> mesh LODs).
	//
	// P0 merges three UE layers into one class:
	//   * UTexture2D         - the asset (source pixels, size, name)
	//   * FTexture2DResource - the render resource
	//   * FTexture2DRHI      - the RHI handle wrapping ID3D12Resource
	// which is the same simplification CMaterial already made for
	// UMaterial + FMaterial. Splitting them back out is what the asset
	// streaming work (FTexture2DStreamIn / UpdateResource) will need.
	//
	// Only RGBA8 sources for now: one channel layout keeps the upload path
	// (CDX12RHITexture2D::Create takes a raw pointer + row pitch) trivial.
	//====================================================================
	class MLENGINE_API CTexture2D : public CStreamableRenderAsset
	{
	public:
		CTexture2D();
		virtual ~CTexture2D();

		// Decodes the file into RGBA8 source pixels. Purely CPU-side, no device
		// needed - callable from a worker thread while the level is parsing.
		bool LoadSourceFromFile(const std::string& FilePath);

		// Uploads the decoded source and creates its SRV. Requires a live device.
		bool InitRHI();
		void ReleaseRHI();

		// True once the GPU texture exists and its SRV is bound to a slot.
		bool HasValidRHI() const;

		CRHITexture2D* GetRHITexture() const { return RHITexture; }

		uint32 GetWidth()  const { return Width; }
		uint32 GetHeight() const { return Height; }
		const std::string& GetSourcePath() const { return SourcePath; }

	private:
		std::string        SourcePath;   // where the source pixels came from
		std::vector<uint8> SourceData;   // RGBA8, row-major, 4 bytes per texel
		uint32             Width = 0;
		uint32             Height = 0;

		// Owned. Destroyed before this asset, which is what keeps the SRV slot
		// returned to the device heap before the heap itself disappears.
		CRHITexture2D* RHITexture = nullptr;
	};
}
