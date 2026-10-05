#pragma once
#include "Refcount.h"
#include "DX12Device.h"

namespace ML
{
	enum class EBufferType : uint8
	{
		Vertex,
		Index,
		Constant,
		Structured
	};

	class CDX12RHIBuffer : public CRefcount 
	{
	public:
		CDX12RHIBuffer() = default;
		CDX12RHIBuffer(ID3D12Resource* InResource);
		virtual ~CDX12RHIBuffer();

		virtual bool Create(EBufferType InType, uint32 InSize, uint32 InStride, const void* InitData);
		virtual void Release();

		D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const ;
		D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const ;
		D3D12_CONSTANT_BUFFER_VIEW_DESC GetConstantBufferViewDesc() const ;

		ID3D12Resource* GetResource() const { return Resource; }
		ID3D12Resource* GetUploadBuffer() const { return UploadBuffer; }

	protected:
		uint32 Size = 0;
		uint32 Stride = 0;

	private:
		ID3D12Resource* Resource = nullptr;
		ID3D12Resource* UploadBuffer = nullptr;
		void* MappedData = nullptr;

		EBufferType Type = EBufferType::Vertex;
	};
}