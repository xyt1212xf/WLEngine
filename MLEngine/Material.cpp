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
		sResType = ResourceType::Material;
		mTypeName = "Material";
	}

	CMaterial::~CMaterial()
	{
		ReleaseRHI();
	}

	bool CMaterial::initialise()
	{
		// Asset-level initialise stays empty: real GPU resources are created in InitRHI.
		// Just push status to loaded here.
		mStatus = ResourceStatus::loaded;
		return true;
	}

	void CMaterial::SetShaders(const std::string& InVSName, const std::string& InPSName)
	{
		VSName = InVSName;
		PSName = InPSName;
	}

	bool CMaterial::InitRHI(const CStaticVertexFactory* InVertexFactory, DXGI_FORMAT DepthFormat)
	{
		ReleaseRHI();

		// 1) InputLayout comes from the VertexFactory; material knows no semantics.
		//    UE counterpart: FGraphicsPipelineStateInitializer merges layout at assembly time.
		if (nullptr == InVertexFactory || !InVertexFactory->HasInputLayout())
		{
			OutputDebugStringA("CMaterial::InitRHI: VertexFactory is null or has no input layout\n");
			return false;
		}
		const std::vector<D3D12_INPUT_ELEMENT_DESC>& InputLayout = InVertexFactory->GetInputLayout();

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

	void CMaterial::ReleaseRHI()
	{
		SafeDelete(PipelineState);
		VertexShader = nullptr;
		PixelShader  = nullptr;
	}
}
