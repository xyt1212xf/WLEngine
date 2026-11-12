#pragma once
#include <d3d12.h>
#include <unordered_map>
#include <mutex>
#include "Common.h"
#include "Refcount.h"
#include "Material.h"   // EBlendMode (full definition). Material.h only forward-declares
					   // CPipelineState, so including it here does NOT create a cycle.

namespace ML
{
	// Describes a root signature's *binding layout* (UE: FRootSignatureDesc).
	// This is orthogonal to PSO raster/depth/blend state: it only says what the
	// shader interface can bind (root constants, SRV/sampler tables). Describing it
	// (instead of enumerating every variant) keeps the set of root signatures
	// open-ended without an ever-growing enum.
	struct FRootSignatureDesc
	{
		static constexpr UINT MaxStaticSamplers = 4;
		static constexpr UINT MaxDynamicSamplers = 4;
		static constexpr UINT DynamicSamplerBaseRegister = MaxStaticSamplers;

		uint32	RootConstantCount       = 0;  // 32-bit root constants at b0 (e.g. 16 for a 4x4 MVP)
		uint32  WorldConstantCount      = 0;  // World at b1 (16)
		uint32	SRVDescriptorCount      = 0;  // SRVs in ONE descriptor table (t0..tN-1), pixel-visible
		uint32	SamplerDescriptorCount  = 0;  // samplers in ONE descriptor table (s0..sN-1), pixel-visible
		uint32  StaticSamplerCount		= 0;
		uint32  LightingCount			= 0; 

		D3D12_STATIC_SAMPLER_DESC  StaticSamplers[MaxStaticSamplers] = {};

		bool operator==(const FRootSignatureDesc& Other) const
		{
			if (RootConstantCount != Other.RootConstantCount ||
				WorldConstantCount != Other.WorldConstantCount ||
				LightingCount != Other.LightingCount ||
				SRVDescriptorCount != Other.SRVDescriptorCount ||
				SamplerDescriptorCount != Other.SamplerDescriptorCount ||
				StaticSamplerCount != Other.StaticSamplerCount)
			{
				return false;
			}
			for (uint32 i = 0; i < StaticSamplerCount; ++i)
			{
				const D3D12_STATIC_SAMPLER_DESC& A = StaticSamplers[i];
				const D3D12_STATIC_SAMPLER_DESC& B = Other.StaticSamplers[i];
				if (A.Filter != B.Filter || A.AddressU != B.AddressU ||
					A.AddressV != B.AddressV || A.AddressW != B.AddressW ||
					A.ShaderRegister != B.ShaderRegister || A.MaxAnisotropy != B.MaxAnisotropy)
				{
					return false;
				}
			}
			return true;
		}
	};

	// UE: FRootSignatureCache / FD3D12RootSignatureManager. Maps a descriptor to a
	// compiled ID3D12RootSignature so identical binding layouts share ONE root
	// signature instead of recompiling/duplicating it per PSO.
	class FRootSignatureCache
	{
	public:
		static FRootSignatureCache& Get();

		// Returns a root signature matching Desc, creating + caching it on first use.
		ID3D12RootSignature* GetOrCreate(ID3D12Device* pDevice, const FRootSignatureDesc& Desc);

		// Releases every cached root signature. Call on device teardown.
		void ReleaseAll();

	private:
		uint64 Hash(const FRootSignatureDesc& Desc) const;

		std::unordered_map<uint64, ID3D12RootSignature*>	mCache;
		std::mutex											mLock;
	};

	class CShader;

	struct FPipelineStateOptions
	{
		bool        bWireframe = false;                              // view-family draw mode
		EBlendMode  BlendMode = EBlendMode::BLEND_Opaque;
		bool        bDepthTest = true;                               // FALSE: ignore the depth buffer
		bool        bDepthWrite = true;                              // FALSE: never write depth
		bool        bCullNone = false;                               // TRUE: fullscreen passes
		D3D12_PRIMITIVE_TOPOLOGY_TYPE TopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	};


	class CDX12PipelineState : public CRefcount
	{
	public:
		CDX12PipelineState() = default;
		virtual ~CDX12PipelineState();

		bool Create(CShader* pVS, CShader* pPS,
			const std::vector<D3D12_INPUT_ELEMENT_DESC>& InputLayout,
			DXGI_FORMAT DepthFormat,
			const FRootSignatureDesc& RootSigDesc,
			bool bWireframe = false,
			EBlendMode InBlendMode = EBlendMode::BLEND_Opaque,
			EMeshPass InPass = EMeshPass::BasePass,
			bool bTwoSided = false);
		void Release();

		ID3D12RootSignature* GetRootSignature() const { return mpRootSignature; }
		ID3D12PipelineState* GetPipelineState() const { return mpPipelineState; }
		bool IsValid() const { return mpPipelineState != nullptr; }

	private:
		// Owned by FRootSignatureCache, not by this PSO (a root signature is shared
		// across every PSO with the same binding layout). Release() must NOT free it.
		ID3D12RootSignature*	mpRootSignature	= nullptr;
		ID3D12PipelineState*	mpPipelineState	= nullptr;
	};
}
