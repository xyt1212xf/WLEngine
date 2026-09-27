#pragma once
#include "Component.h"
#include "DX12Device.h"

namespace ML
{
	enum class EVertexElementType : uint8
	{
		Float1,
		Float2,
		Float3,
		Float4,
		Uint4,
		Ushort4,
		Ubyte4,
		PackedNormal, 
	};

	struct VertexStreamUnit
	{
		ID3D12Resource* VertexBuffer = nullptr;
		uint32 Stride = 0;
		uint32 Offset = 0;
		uint32 Slot = 0;  // input slot
		EVertexElementType Type = EVertexElementType::Float3;
		D3D12_INPUT_CLASSIFICATION Classification = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		uint32 InstanceStepRate = 0;

		DXGI_FORMAT GetDXGIFormat() const
		{
			switch (Type)
			{
			case EVertexElementType::Float1:  return DXGI_FORMAT_R32_FLOAT;
			case EVertexElementType::Float2:  return DXGI_FORMAT_R32G32_FLOAT;
			case EVertexElementType::Float3:  return DXGI_FORMAT_R32G32B32_FLOAT;
			case EVertexElementType::Float4:  return DXGI_FORMAT_R32G32B32A32_FLOAT;
			case EVertexElementType::Uint4:   return DXGI_FORMAT_R32G32B32A32_UINT;
			case EVertexElementType::Ushort4: return DXGI_FORMAT_R16G16B16A16_UINT;
			case EVertexElementType::Ubyte4:  return DXGI_FORMAT_R8G8B8A8_UNORM;
		//	case EVertexElementType::PackedNormal: return DXGI_FORMAT_R10G10B10A2_UNORM;
			default: return DXGI_FORMAT_UNKNOWN;
			}
		}
	};

	struct InputElement
	{
		const char* SemanticName = nullptr;
		uint32 SemanticIndex = 0;
		EVertexElementType Type = EVertexElementType::Float1;
		uint32 Slot = 0;
		uint32 AlignedByteOffset = 0;
		D3D12_INPUT_CLASSIFICATION Classification = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		uint32 InstanceStepRate = 0;
	};

	class CVertexStreamFactory  	
	{
	public:
		CVertexStreamFactory();
		virtual ~CVertexStreamFactory();
	};

	class CStaticVertexFactory : public CVertexStreamFactory
	{
	public:
		CStaticVertexFactory();
		virtual ~CStaticVertexFactory();
	};
}