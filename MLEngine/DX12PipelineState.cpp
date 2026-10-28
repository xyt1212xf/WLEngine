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

	bool CDX12PipelineState::Create(CShader* pVS, CShader* pPS,
		const std::vector<D3D12_INPUT_ELEMENT_DESC>& InputLayout,
		DXGI_FORMAT DepthFormat,
		bool bWireframe)
	{
		Release();

		CDX12RHIDevice* pRHIDevice = GEngine->GetPlugs<CGraphicPlug>()->GetDevice();
		ID3D12Device* pDevice = pRHIDevice ? pRHIDevice->GetDevice() : nullptr;
		if (nullptr == pDevice || nullptr == pVS || nullptr == pPS)
		{
			return false;
		}

		ID3DBlob* pRootSigBlob = pVS->ExtractRootSignature();
		if (nullptr == pRootSigBlob)
		{
			return false;
		}
		HRESULT hr = pDevice->CreateRootSignature(
			0,
			pRootSigBlob->GetBufferPointer(),
			pRootSigBlob->GetBufferSize(),
			IID_PPV_ARGS(&mpRootSignature));
		SafeRelease(pRootSigBlob);
		if (FAILED(hr))
		{
			return false;
		}

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
