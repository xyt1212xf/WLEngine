#pragma once
#include "RHIbuffer.h"
#include "DX12Device.h"

namespace ML
{
	class CDX12RHIbuffer : public CRHIBuffer
	{
	public:
		CDX12RHIbuffer(ID3D12Resource* InResource);
		virtual ~CDX12RHIbuffer();
		ID3D12Resource* GetResource() const { return Resource; }

	private:
		ID3D12Resource* Resource = nullptr;
	};
}