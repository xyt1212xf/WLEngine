#pragma once
#include "Common.h"
#include "Refcount.h"
#include <d3d12.h>

namespace ML
{
	// Root signature variants, defined in C++ (see CDX12PipelineState::BuildRootSignature).
	// UE keeps root signatures as engine-side resources rather than embedding them in
	// shader bytecode; the HLSL only declares its register bindings (b0/t0/s0).
	enum class ERootSignatureType : uint8
	{
		SolidColor = 0,  // 1 root constant: b0, 16x32-bit MVP matrix
		Textured,        // + SRV table (t0) and Sampler table (s0), both pixel-visible
	};

	class CShader;
	class CDX12PipelineState : public CRefcount
	{
	public:
		CDX12PipelineState() = default;
		virtual ~CDX12PipelineState();

		bool Create(CShader* pVS, CShader* pPS,
					const std::vector<D3D12_INPUT_ELEMENT_DESC>& InputLayout,
					DXGI_FORMAT DepthFormat,
					ERootSignatureType RootSigType,
					bool bWireframe = false);
		void Release();

		ID3D12RootSignature* GetRootSignature() const { return mpRootSignature; }
		ID3D12PipelineState* GetPipelineState() const { return mpPipelineState; }
		bool IsValid() const { return mpPipelineState != nullptr; }

	private:
		ID3D12RootSignature*	mpRootSignature	= nullptr;
		ID3D12PipelineState*	mpPipelineState	= nullptr;
	};
}
