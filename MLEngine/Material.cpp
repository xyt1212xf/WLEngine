#include "Material.h"
#include "Shader.h"
#include "GlobalShader.h"
#include "ShaderMgr.h"
#include "MLEngine.h"
#include "PipelineState.h"
#include "Refcount.h"
#include "VertexStreamFactory.h"

namespace ML
{
	CMaterial::CMaterial()
	{
	}

	CMaterial::~CMaterial()
	{
		ReleasePSO();
	}

	bool CMaterial::initialise()
	{
		// Asset-level initialise stays empty: real GPU resources are created in InitRHI.
		// Just push status to loaded here.
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

	bool CMaterial::InitPSO(const CStaticVertexFactory* InVertexFactory, DXGI_FORMAT DepthFormat)
	{
		ReleasePSO();

		// 1) Layout resolution:
		//    priority 1: VertexFactory's layout (mesh-driven path)
		//    priority 2: material's own mDefaultInputLayout (fallback for Default Material at boot)
		//    UE counterpart: FGraphicsPipelineStateInitializer merges layout at assembly time.
		const std::vector<D3D12_INPUT_ELEMENT_DESC>* LayoutPtr = nullptr;
		if (InVertexFactory && InVertexFactory->HasInputLayout())
		{
			LayoutPtr = &InVertexFactory->GetInputLayout();
		}
		else if (!mDefaultInputLayout.empty())
		{
			LayoutPtr = &mDefaultInputLayout;
		}

		if (nullptr == LayoutPtr)
		{
			OutputDebugStringA("CMaterial::InitRHI: neither VertexFactory layout nor DefaultInputLayout available\n");
			return false;
		}
		const std::vector<D3D12_INPUT_ELEMENT_DESC>& InputLayout = *LayoutPtr;

		// 2) Get compiled shaders via the engine's CShaderMgr (a lookup, not a new).
		//    Timing: CEngine::Initialise has already Init'd the ShaderMgr by now.
		CShaderMgr* ShaderMgr = GEngine ? GEngine->GetMgr<CShaderMgr>() : nullptr;
		if (nullptr == ShaderMgr)
		{
			OutputDebugStringA("CMaterial::InitRHI: GEngine->GetShaderMgr() is null. Engine not initialised?\n");
			return false;
		}
		VertexShader = ShaderMgr->GetShader(VSName);
		PixelShader  = ShaderMgr->GetShader(PSName);
		if (nullptr == VertexShader || nullptr == PixelShader)
		{
			OutputDebugStringA(("CMaterial::InitRHI: missing shader " + VSName + " / " + PSName + "\n").c_str());
			return false;
		}

		// 3) Create the PSO. This is the only time a material creates GPU resources; read-only afterwards.
		PipelineState = new CPipelineState();
		if (!PipelineState->Create(VertexShader, PixelShader, InputLayout, DepthFormat))
		{
			SafeDelete(PipelineState);
			return false;
		}
		return true;
	}

	void CMaterial::ReleasePSO()
	{
		SafeDelete(PipelineState);
		VertexShader = nullptr;
		PixelShader  = nullptr;
	}
}
