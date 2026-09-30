#pragma once
#include "RHIbuffer.h"
#include "DX12Device.h"

namespace ML
{
	class CDX12RHIBuffer : public CRHIBuffer
	{
	public:
		CDX12RHIBuffer(ID3D12Resource* InResource);
		virtual ~CDX12RHIBuffer();
		ID3D12Resource* GetResource() const { return Resource; }

	private:
		ID3D12Resource* Resource = nullptr;
	};
}