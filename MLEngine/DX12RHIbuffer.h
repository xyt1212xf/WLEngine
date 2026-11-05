#pragma once
#include "Refcount.h"
#include "Device.h"

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

		D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView();
		D3D12_INDEX_BUFFER_VIEW GetIndexBufferView();
		D3D12_CONSTANT_BUFFER_VIEW_DESC GetConstantBufferViewDesc();

		void UpdateData(const void* InData, uint32 InBytes);
		void* GetMappedData() const { return MappedData; }

		ID3D12Resource* GetResource() const { return mResource; }
		ID3D12Resource* GetUploadBuffer() const { return UploadBuffer; }

	protected:
		uint32 mDataSize = 0;
		uint32 mStride = 0;

	private:
		D3D12_VERTEX_BUFFER_VIEW VertexView = {};
		D3D12_INDEX_BUFFER_VIEW IndexView = {};
		D3D12_CONSTANT_BUFFER_VIEW_DESC ViewDesc = {};

		ID3D12Resource* mResource = nullptr;
		ID3D12Resource* UploadBuffer = nullptr;
		void* MappedData = nullptr;

		EBufferType mType = EBufferType::Vertex;
	};
}
