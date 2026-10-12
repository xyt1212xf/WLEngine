#pragma once
#include "Common.h"
#include "Refcount.h"
#include "Array.h"
#include <d3d12.h>

namespace ML
{
	class CShader;
	class CDX12PipelineState : public CRefcount
	{
	public:
		CDX12PipelineState() = default;
		virtual ~CDX12PipelineState();

		bool Create(CShader* pVS, CShader* pPS,
					const TArray<D3D12_INPUT_ELEMENT_DESC>& InputLayout,
					DXGI_FORMAT DepthFormat);
		void Release();

		ID3D12RootSignature* GetRootSignature() const { return mpRootSignature; }
		ID3D12PipelineState* GetPipelineState() const { return mpPipelineState; }
		bool IsValid() const { return mpPipelineState != nullptr; }

	private:
		ID3D12RootSignature*	mpRootSignature	= nullptr;
		ID3D12PipelineState*	mpPipelineState	= nullptr;
	};
}
