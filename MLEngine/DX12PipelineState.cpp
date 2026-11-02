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

	// Builds the root signature in C++ rather than extracting it from embedded shader
	// bytecode. UE keeps root signatures as engine-side resources; the HLSL only
	// declares its register bindings (register(b0/t0/s0)). Root parameter order is
	// fixed to match CRenderer's binding calls:
	//   [0] MVP 32-bit constants (b0), [1] SRV table (t0), [2] Sampler table (s0).
	static ID3D12RootSignature* BuildRootSignature(ID3D12Device* pDevice, ERootSignatureType Type)
	{
		D3D12_ROOT_PARAMETER Params[3] = {};
		UINT NumParams = 0;

		// [0] MVP as 16 32-bit root constants bound to b0 (all shader stages).
		D3D12_ROOT_PARAMETER& MVPParam = Params[NumParams++];
		MVPParam.ParameterType		= D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
		MVPParam.ShaderVisibility	= D3D12_SHADER_VISIBILITY_ALL;
		MVPParam.Constants.ShaderRegister		= 0;  // b0
		MVPParam.Constants.RegisterSpace		= 0;
		MVPParam.Constants.Num32BitValues	= 16;

		if (Type == ERootSignatureType::Textured)
		{
			// [1] SRV descriptor table for the base-color texture (t0, pixel-visible).
			D3D12_DESCRIPTOR_RANGE SRVRange = {};
			SRVRange.RangeType							= D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
			SRVRange.NumDescriptors						= 1;
			SRVRange.BaseShaderRegister					= 0;  // t0
			SRVRange.RegisterSpace						= 0;
			SRVRange.OffsetInDescriptorsFromTableStart	= D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

			D3D12_ROOT_PARAMETER& SRVParam = Params[NumParams++];
			SRVParam.ParameterType					= D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			SRVParam.ShaderVisibility				= D3D12_SHADER_VISIBILITY_PIXEL;
			SRVParam.DescriptorTable.NumDescriptorRanges	= 1;
			SRVParam.DescriptorTable.pDescriptorRanges	= &SRVRange;

			// [2] Sampler descriptor table for the static sampler (s0, pixel-visible).
			D3D12_DESCRIPTOR_RANGE SamplerRange = {};
			SamplerRange.RangeType						= D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
			SamplerRange.NumDescriptors					= 1;
			SamplerRange.BaseShaderRegister				= 0;  // s0
			SamplerRange.RegisterSpace					= 0;
			SamplerRange.OffsetInDescriptorsFromTableStart	= D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

			D3D12_ROOT_PARAMETER& SamplerParam = Params[NumParams++];
			SamplerParam.ParameterType				= D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			SamplerParam.ShaderVisibility			= D3D12_SHADER_VISIBILITY_PIXEL;
			SamplerParam.DescriptorTable.NumDescriptorRanges	= 1;
			SamplerParam.DescriptorTable.pDescriptorRanges	= &SamplerRange;
		}

		D3D12_ROOT_SIGNATURE_DESC Desc = {};
		Desc.NumParameters		= NumParams;
		Desc.pParameters		= Params;
		Desc.NumStaticSamplers	= 0;
		Desc.pStaticSamplers	= nullptr;
		Desc.Flags				= D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		ID3DBlob* pBlob		= nullptr;
		ID3DBlob* pError	= nullptr;
		HRESULT hr = D3D12SerializeRootSignature(&Desc, D3D_ROOT_SIGNATURE_VERSION_1, &pBlob, &pError);
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

	bool CDX12PipelineState::Create(CShader* pVS, CShader* pPS,
		const std::vector<D3D12_INPUT_ELEMENT_DESC>& InputLayout,
		DXGI_FORMAT DepthFormat,
		ERootSignatureType RootSigType,
		bool bWireframe)
	{
		Release();

		CDX12RHIDevice* pRHIDevice = GEngine->GetPlugs<CGraphicPlug>()->GetDevice();
		ID3D12Device* pDevice = pRHIDevice ? pRHIDevice->GetDevice() : nullptr;
		if (nullptr == pDevice || nullptr == pVS || nullptr == pPS)
		{
			return false;
		}

		mpRootSignature = BuildRootSignature(pDevice, RootSigType);
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
		SafeRelease(mpRootSignature);
	}
}
