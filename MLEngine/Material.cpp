#include "Material.h"
#include "Texture2D.h"
#include "Shader.h"
#include "GlobalShader.h"
#include "ShaderMgr.h"
#include "MLEngine.h"
#include "PipelineState.h"
#include "Refcount.h"
#include "VertexStreamFactory.h"
#include <cstring>
#include <cassert>

namespace ML
{
	// P0 keeps one depth format engine-wide. The renderer owns that choice in UE
	// (FGraphicsPipelineStateInitializer carries the render target state);
	// the material only needs to remember what it built against.
	static constexpr DXGI_FORMAT DefaultDepthFormat = DXGI_FORMAT_D32_FLOAT;

	// FNV-1a over the layout parts that actually change the compiled vertex stage.
	// The semantic NAME must be part of it: POSITION@slot0 and TEXCOORD@slot0 are
	// different programs even though every numeric field matches.
	static uint64 HashInputLayout(const std::vector<D3D12_INPUT_ELEMENT_DESC>& Layout)
	{
		uint64 Hash = 0xcbf29ce484222325ull;

		auto MixBytes = [&Hash](const void* Data, size_t Size)
		{
			const uint8* Bytes = reinterpret_cast<const uint8*>(Data);
			for (size_t i = 0; i < Size; ++i)
			{
				Hash ^= Bytes[i];
				Hash *= 0x100000001b3ull;
			}
		};

		for (const D3D12_INPUT_ELEMENT_DESC& Element : Layout)
		{
			MixBytes(Element.SemanticName, strlen(Element.SemanticName));
			MixBytes(&Element.SemanticIndex, sizeof(Element.SemanticIndex));
			MixBytes(&Element.Format, sizeof(Element.Format));
			MixBytes(&Element.InputSlot, sizeof(Element.InputSlot));
			MixBytes(&Element.AlignedByteOffset, sizeof(Element.AlignedByteOffset));
			MixBytes(&Element.InputSlotClass, sizeof(Element.InputSlotClass));
			MixBytes(&Element.InstanceDataStepRate, sizeof(Element.InstanceDataStepRate));
		}
		return Hash;
	}

	CMaterial::CMaterial()
	{
	}

	CMaterial::~CMaterial()
	{
		ReleasePipelineStates();
	}

	bool CMaterial::initialise()
	{
		// Asset-level initialise stays empty: real GPU resources are created when
		// a draw first asks for the pipeline state.
		return true;
	}

	void CMaterial::SetShaders(const std::string& InVSName, const std::string& InPSName)
	{
		mVSName = InVSName;
		mPSName = InPSName;
	}

	void CMaterial::SetDefaultInputLayout(std::initializer_list<D3D12_INPUT_ELEMENT_DESC> Elements)
	{
		mDefaultInputLayout.assign(Elements.begin(), Elements.end());
	}

	void CMaterial::SetDefaultInputLayout(std::vector<D3D12_INPUT_ELEMENT_DESC>&& Elements)
	{
		mDefaultInputLayout = std::move(Elements);
	}

	void CMaterial::SetBaseColorTexture(CTexture2D* InTexture)
	{
		BaseColorTexture = InTexture;
	}

	bool CMaterial::HasBaseColorTexture() const
	{
		return nullptr != BaseColorTexture && BaseColorTexture->HasValidRHI();
	}

	const std::vector<D3D12_INPUT_ELEMENT_DESC>* CMaterial::SelectInputLayout(const CStaticVertexFactory* InVertexFactory) const
	{
		// Layout resolution order (see Material.h):
		//   priority 1: VertexFactory's layout (mesh-driven path)
		//   priority 2: material's own mDefaultInputLayout (fallback for Default Material at boot)
		// UE counterpart: FGraphicsPipelineStateInitializer merges layout at assembly time.
		if (InVertexFactory && InVertexFactory->HasInputLayout())
		{
			return &InVertexFactory->GetInputLayout();
		}
		if (!mDefaultInputLayout.empty())
		{
			return &mDefaultInputLayout;
		}
		return nullptr;
	}

	CPipelineState* CMaterial::GetPipelineState(const CStaticVertexFactory* InVertexFactory, bool bWireframe)
	{
		const std::vector<D3D12_INPUT_ELEMENT_DESC>* LayoutPtr = SelectInputLayout(InVertexFactory);
		if (nullptr == LayoutPtr)
		{
			return nullptr;
		}

		const uint64 LayoutKey = HashInputLayout(*LayoutPtr);
		// Fold the draw mode AND the blend mode into the cache key: a wireframe PSO
		// must not collide with its solid twin, and an opaque vs translucent PSO
		// (same vertex layout, different blend state / depth-write mask) must not
		// share either. The per-field avalanche constants keep the three sources
		// from colliding.
		const uint64 Key = LayoutKey
			^ (bWireframe ? 0x9E3779B97F4A7C15ull : 0ull)
			^ (static_cast<uint64>(BlendMode) * 0xC2B2AE3D27D4EB4Full);

		{
			std::lock_guard<std::mutex> Lock(mPSOMutex);
			auto It = mPSOCache.find(Key);
			if (It != mPSOCache.end())
			{
				return It->second;
			}
		}

		// Build outside the lock: compiling/baking a PSO touches the device and may
		// take a while; blocking the renderer's first-frame lookup on a peer's
		// creation would serialize startup for no reason.
		CPipelineState* PSO = CreatePipelineState(InVertexFactory, bWireframe);
		if (nullptr == PSO)
		{
			return nullptr;
		}

		std::lock_guard<std::mutex> Lock(mPSOMutex);
		auto Inserted = mPSOCache.emplace(Key, PSO);
		if (!Inserted.second)
		{
			// Another thread built the same layout while this one was creating.
			// This is the only place a genuine duplicate (same layout key) is detected:
			// a PSO for this material+layout was already in the cache. Log it and
			// assert so a redundant build (e.g. a race the code is meant to prevent)
			// surfaces during debugging instead of being silently discarded.
			OutputDebugStringA(
				("CMaterial::GetPipelineState: DUPLICATE PSO built for layout key 0x" +
				std::to_string(Key) + " (VS=" + mVSName + " PS=" + mPSName + ") - "
				"discards the redundant copy. A second call raced the first build.\n").c_str());
			assert(false && "CMaterial::GetPipelineState: duplicate PSO for an already-cached layout key");
			SafeDelete(PSO);
			return Inserted.first->second;
		}
		return PSO;
	}

	CPipelineState* CMaterial::CreatePipelineState(const CStaticVertexFactory* InVertexFactory, bool bWireframe)
	{
		const std::vector<D3D12_INPUT_ELEMENT_DESC>* LayoutPtr = SelectInputLayout(InVertexFactory);
		if (nullptr == LayoutPtr)
		{
			OutputDebugStringA("CMaterial::CreatePipelineState: neither VertexFactory layout nor DefaultInputLayout available\n");
			return nullptr;
		}
		const std::vector<D3D12_INPUT_ELEMENT_DESC>& InputLayout = *LayoutPtr;

		// Get compiled shaders via the engine's CShaderMgr (a lookup, not a new).
		// Timing: CEngine::Initialise has already Init'd the ShaderMgr by now.
		CShaderMgr* ShaderMgr = GEngine ? GEngine->GetMgr<CShaderMgr>() : nullptr;
		if (nullptr == ShaderMgr)
		{
			OutputDebugStringA("CMaterial::CreatePipelineState: GEngine->GetShaderMgr() is null. Engine not initialised?\n");
			return nullptr;
		}
		mVertexShader = ShaderMgr->GetShader(mVSName);
		mPixelShader  = ShaderMgr->GetShader(mPSName);
		if (nullptr == mVertexShader || nullptr == mPixelShader)
		{
			OutputDebugStringA(("CMaterial::CreatePipelineState: missing shader " + mVSName + " / " + mPSName + "\n").c_str());
			return nullptr;
		}

		// The root signature is derived from what the material binds (UE: the
		// FRootSignatureDesc is built from the material's parameter interface), not from
		// an enum of variants. A textured material adds the SRV/Sampler tables on top of
		// the shared MVP constant; a plain one keeps just the constant.
		FRootSignatureDesc RootSigDesc;
		RootSigDesc.RootConstantCount = 16;   // 4x4 MVP at b0
		RootSigDesc.WorldConstantCount = 16;   // 4x4 MVP at b0
		RootSigDesc.LightingConstantCount = 16;   // 4x4 MVP at b0

		RootSigDesc.SRVDescriptorCount     = bUsesBaseColorTexture ? 1u : 0u;
		RootSigDesc.SamplerDescriptorCount = bUsesBaseColorTexture ? FRootSignatureDesc::MaxDynamicSamplers : 0u;


		auto FillStaticSampler = [&](UINT Slot, D3D12_FILTER Filter,
			D3D12_TEXTURE_ADDRESS_MODE Addr, UINT ShaderReg)
			{
				D3D12_STATIC_SAMPLER_DESC& S = RootSigDesc.StaticSamplers[Slot];
				S.Filter = Filter;
				S.AddressU = S.AddressV = S.AddressW = Addr;
				S.MipLODBias = 0.0f;
				S.MaxAnisotropy = (Filter == D3D12_FILTER_ANISOTROPIC) ? 4u : 1u;
				S.ComparisonFunc = D3D12_COMPARISON_FUNC_LESS;
				S.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
				S.MinLOD = 0.0f; S.MaxLOD = D3D12_FLOAT32_MAX;
				S.ShaderRegister = ShaderReg;
				S.RegisterSpace = 0;
				S.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
			};
		// s0 必须与 HLSL 的 register(s0) 对应(基色/基纹理采样 = linear wrap)
		FillStaticSampler(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_WRAP, 0);
		FillStaticSampler(1, D3D12_FILTER_MIN_MAG_MIP_POINT, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, 1);
		FillStaticSampler(2, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, 2);
		FillStaticSampler(3, D3D12_FILTER_ANISOTROPIC, D3D12_TEXTURE_ADDRESS_MODE_WRAP, 3);
		RootSigDesc.StaticSamplerCount = 4;

		// Create the PSO. This is where a material creates GPU resources.
		CPipelineState* PSO = new CPipelineState();
		if (!PSO->Create(mVertexShader, mPixelShader, InputLayout, DefaultDepthFormat, RootSigDesc, bWireframe, BlendMode))
		{
			OutputDebugStringA(("CMaterial::CreatePipelineState: PSO creation failed for " + mVSName + " / " + mPSName + "\n").c_str());
			SafeDelete(PSO);
			return nullptr;
		}

		return PSO;
	}

	void CMaterial::ReleasePipelineStates()
	{
		std::lock_guard<std::mutex> Lock(mPSOMutex);
		for (auto& Pair : mPSOCache)
		{
			SafeDelete(Pair.second);
		}
		mPSOCache.clear();
		mVertexShader = nullptr;
		mPixelShader  = nullptr;
	}
}
