#include "DX12PipelineState.h"
#include "Shader.h"
#include "DX12Shader.h"
#include "Device.h"
#include "GraphicPlug.h"
#include "MLEngine.h"

namespace ML
{
	CDX12PipelineState::~CDX12PipelineState()
	{
		Release();
	}

	// Builds a root signature in C++ from a descriptor (UE: FRootSignatureDesc ->
	// FD3D12RootSignature). Root parameter order is fixed to match CRenderer's
	// binding calls: [0] MVP 32-bit constants (b0), [1] SRV table (t0),
	// [2] Sampler table (s0). The HLSL only declares register(b0/t0/s0); it never
	// embeds a root signature.
	static ID3D12RootSignature* BuildRootSignature(ID3D12Device* pDevice, const FRootSignatureDesc& Desc)
	{
		D3D12_ROOT_PARAMETER Params[3] = {};
		UINT NumParams = 0;

		// [0] MVP (or other) as 32-bit root constants bound to b0 (all shader stages).
		if (Desc.RootConstantCount > 0)
		{
			D3D12_ROOT_PARAMETER& MVPParam = Params[NumParams++];
			MVPParam.ParameterType		= D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
			MVPParam.ShaderVisibility	= D3D12_SHADER_VISIBILITY_ALL;
			MVPParam.Constants.ShaderRegister		= 0;  // b0
			MVPParam.Constants.RegisterSpace		= 0;
			MVPParam.Constants.Num32BitValues		= Desc.RootConstantCount;
		}

		if (Desc.SRVDescriptorCount > 0)
		{
			// [1] One SRV descriptor table covering t0..t(N-1), pixel-visible. N contiguous
			// SRVs let the shader bind Texture2D gTex[N] : register(t0) (or register(t0..t7)).
			D3D12_DESCRIPTOR_RANGE SRVRange = {};
			SRVRange.RangeType							= D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
			SRVRange.NumDescriptors						= Desc.SRVDescriptorCount;
			SRVRange.BaseShaderRegister					= 0;  // t0
			SRVRange.RegisterSpace						= 0;
			SRVRange.OffsetInDescriptorsFromTableStart	= D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

			D3D12_ROOT_PARAMETER& SRVParam = Params[NumParams++];
			SRVParam.ParameterType					= D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			SRVParam.ShaderVisibility				= D3D12_SHADER_VISIBILITY_PIXEL;
			SRVParam.DescriptorTable.NumDescriptorRanges	= 1;
			SRVParam.DescriptorTable.pDescriptorRanges	= &SRVRange;
		}

		if (Desc.SamplerDescriptorCount > 0)
		{
			// [2] One sampler descriptor table covering s0..s(N-1), pixel-visible.
			D3D12_DESCRIPTOR_RANGE SamplerRange = {};
			SamplerRange.RangeType						= D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
			SamplerRange.NumDescriptors					= Desc.SamplerDescriptorCount;
			SamplerRange.BaseShaderRegister				= 0;  // s0
			SamplerRange.RegisterSpace					= 0;
			SamplerRange.OffsetInDescriptorsFromTableStart	= D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

			D3D12_ROOT_PARAMETER& SamplerParam = Params[NumParams++];
			SamplerParam.ParameterType				= D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			SamplerParam.ShaderVisibility			= D3D12_SHADER_VISIBILITY_PIXEL;
			SamplerParam.DescriptorTable.NumDescriptorRanges	= 1;
			SamplerParam.DescriptorTable.pDescriptorRanges	= &SamplerRange;
		}

		D3D12_ROOT_SIGNATURE_DESC DescNative = {};
		DescNative.NumParameters		= NumParams;
		DescNative.pParameters			= Params;
		DescNative.NumStaticSamplers	= 0;
		DescNative.pStaticSamplers		= nullptr;
		DescNative.Flags				= D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		ID3DBlob* pBlob		= nullptr;
		ID3DBlob* pError	= nullptr;
		HRESULT hr = D3D12SerializeRootSignature(&DescNative, D3D_ROOT_SIGNATURE_VERSION_1, &pBlob, &pError);
		if (FAILED(hr))
		{
			if (pError)
			{
				OutputDebugStringA(static_cast<const char*>(pError->GetBufferPointer()));
				SafeRelease(pError);
			}
			return nullptr;
		}
		ID3D12RootSignature* pRootSig = nullptr;
		hr = pDevice->CreateRootSignature(0, pBlob->GetBufferPointer(), pBlob->GetBufferSize(), IID_PPV_ARGS(&pRootSig));
		SafeRelease(pBlob);
		return SUCCEEDED(hr) ? pRootSig : nullptr;
	}

	FRootSignatureCache& FRootSignatureCache::Get()
	{
		static FRootSignatureCache Instance;
		return Instance;
	}

	uint64 FRootSignatureCache::Hash(const FRootSignatureDesc& Desc) const
	{
		// hash_combine over the three descriptor fields into a 64-bit key. Using the
		// counts (not just present/absent) so e.g. 1 SRV vs 8 SRVs yield distinct keys.
		auto Combine = [](uint64 Seed, uint32 Val)
		{
			return Seed ^ (static_cast<uint64>(Val) + 0x9E3779B97F4A7C15ULL + (Seed << 6) + (Seed >> 2));
		};
		uint64 Key = 0;
		Key = Combine(Key, Desc.RootConstantCount);
		Key = Combine(Key, Desc.SRVDescriptorCount);
		Key = Combine(Key, Desc.SamplerDescriptorCount);
		return Key;
	}

	ID3D12RootSignature* FRootSignatureCache::GetOrCreate(ID3D12Device* pDevice, const FRootSignatureDesc& Desc)
	{
		const uint64 Key = Hash(Desc);
		{
			std::lock_guard<std::mutex> Lock(mLock);
			auto It = mCache.find(Key);
			if (It != mCache.end())
			{
				return It->second;
			}
		}
		// Build outside the lock: serializing a root signature only touches the device.
		ID3D12RootSignature* pRootSig = BuildRootSignature(pDevice, Desc);
		if (nullptr == pRootSig)
		{
			return nullptr;
		}
		std::lock_guard<std::mutex> Lock(mLock);
		// Another thread may have built the same descriptor while we were outside the lock.
		auto Emplaced = mCache.emplace(Key, pRootSig);
		if (!Emplaced.second)
		{
			SafeRelease(pRootSig);  // lost the race; keep the winner's copy
			return Emplaced.first->second;
		}
		return pRootSig;
	}

	void FRootSignatureCache::ReleaseAll()
	{
		std::unordered_map<uint64, ID3D12RootSignature*>	TempCache;
		{
			std::lock_guard<std::mutex> Lock(mLock);
			std::swap(TempCache, mCache);
		}
		for (auto& Pair : TempCache)
		{
			SafeRelease(Pair.second);
		}
	}

	bool CDX12PipelineState::Create(CShader* pVS, CShader* pPS,
		const std::vector<D3D12_INPUT_ELEMENT_DESC>& InputLayout,
		DXGI_FORMAT DepthFormat,
		const FRootSignatureDesc& RootSigDesc,
		bool bWireframe)
	{
		Release();

		CDX12RHIDevice* pRHIDevice = GEngine->GetPlugs<CGraphicPlug>()->GetDevice();
		ID3D12Device* pDevice = pRHIDevice ? pRHIDevice->GetDevice() : nullptr;
		if (nullptr == pDevice || nullptr == pVS || nullptr == pPS)
		{
			return false;
		}

		// Root signature comes from the cache, keyed by its binding-layout descriptor.
		mpRootSignature = FRootSignatureCache::Get().GetOrCreate(pDevice, RootSigDesc);
		if (nullptr == mpRootSignature)
		{
			return false;
		}

		HRESULT hr;
		D3D12_GRAPHICS_PIPELINE_STATE_DESC PSODesc = {};

		PSODesc.pRootSignature	= mpRootSignature;
		PSODesc.VS				= pVS->GetShaderByteCode();
		PSODesc.PS				= pPS->GetShaderByteCode();

		PSODesc.InputLayout = { InputLayout.data(), (UINT)InputLayout.size() };

		// UE: the fill mode is part of the PSO, driven by the view family's
		// EngineShowFlags.Wireframe (a draw-mode override, not a material property).
		PSODesc.RasterizerState.FillMode				= bWireframe ? D3D12_FILL_MODE_WIREFRAME : D3D12_FILL_MODE_SOLID;
		PSODesc.RasterizerState.CullMode				= D3D12_CULL_MODE_BACK;
		PSODesc.RasterizerState.FrontCounterClockwise	= TRUE;
		PSODesc.RasterizerState.DepthClipEnable			= TRUE;

		PSODesc.BlendState.RenderTarget[0].SrcBlend			= D3D12_BLEND_ONE;
		PSODesc.BlendState.RenderTarget[0].DestBlend		= D3D12_BLEND_ZERO;
		PSODesc.BlendState.RenderTarget[0].BlendOp			= D3D12_BLEND_OP_ADD;
		PSODesc.BlendState.RenderTarget[0].SrcBlendAlpha	= D3D12_BLEND_ONE;
		PSODesc.BlendState.RenderTarget[0].DestBlendAlpha	= D3D12_BLEND_ZERO;
		PSODesc.BlendState.RenderTarget[0].BlendOpAlpha		= D3D12_BLEND_OP_ADD;
		PSODesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

		PSODesc.DepthStencilState.DepthEnable		= TRUE;
		PSODesc.DepthStencilState.DepthWriteMask	= D3D12_DEPTH_WRITE_MASK_ALL;
		PSODesc.DepthStencilState.DepthFunc			= D3D12_COMPARISON_FUNC_LESS;

		PSODesc.SampleMask				= UINT_MAX;
		PSODesc.PrimitiveTopologyType	= D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		PSODesc.NumRenderTargets		= 1;
		PSODesc.RTVFormats[0]			= DXGI_FORMAT_R8G8B8A8_UNORM;
		PSODesc.DSVFormat				= DepthFormat;
		PSODesc.SampleDesc.Count		= 1;

		hr = pDevice->CreateGraphicsPipelineState(&PSODesc, IID_PPV_ARGS(&mpPipelineState));
		return SUCCEEDED(hr);
	}

	void CDX12PipelineState::Release()
	{
		SafeRelease(mpPipelineState);
		// mpRootSignature is owned by FRootSignatureCache (shared across every PSO with
		// the same binding layout), so it is NOT freed here.
		mpRootSignature = nullptr;
	}
}
