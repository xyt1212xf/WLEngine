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
		VSName = InVSName;
		PSName = InPSName;
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
		// Fold the draw mode into the cache key: a wireframe PSO must not collide
		// with its solid twin (same layout). The low-bit avalanche constant keeps
		// the two keys distinct without disturbing the layout hash's own bits.
		const uint64 Key = LayoutKey ^ (bWireframe ? 0x9E3779B97F4A7C15ull : 0ull);

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
				std::to_string(Key) + " (VS=" + VSName + " PS=" + PSName + ") - "
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
		VertexShader = ShaderMgr->GetShader(VSName);
		PixelShader  = ShaderMgr->GetShader(PSName);
		if (nullptr == VertexShader || nullptr == PixelShader)
		{
			OutputDebugStringA(("CMaterial::CreatePipelineState: missing shader " + VSName + " / " + PSName + "\n").c_str());
			return nullptr;
		}

		// Create the PSO. This is where a material creates GPU resources.
		CPipelineState* PSO = new CPipelineState();
		if (!PSO->Create(VertexShader, PixelShader, InputLayout, DefaultDepthFormat, bWireframe))
		{
			OutputDebugStringA(("CMaterial::CreatePipelineState: PSO creation failed for " + VSName + " / " + PSName + "\n").c_str());
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
		VertexShader = nullptr;
		PixelShader  = nullptr;
	}
}
