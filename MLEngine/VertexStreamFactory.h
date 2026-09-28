#pragma once
#include "Component.h"
#include "DX12Device.h"
#include "Array.h"

namespace ML
{
	enum class EVertexAttrib : uint8
	{
		Position,
		Normal,
		Tangent,
		UV0,
		UV1,
		UV2,
		UV3,
		Color,
		BoneIndex,
		BoneWeight
	};

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

	struct SDataStreamUnit
	{
		ID3D12Resource* Buffer = nullptr; // GPU buffer
		uint32 Slot = 0;                  // input slot
		uint32 Offset = 0;               // byte offset in vertex / buffer
		uint32 Stride = 0;               // per-vertex stride
		DXGI_FORMAT Format = DXGI_FORMAT_UNKNOWN;
		bool bValid = false;
	};

	struct SMeshVertexSchema
	{
		SDataStreamUnit Position;
		SDataStreamUnit Normal;
		SDataStreamUnit Tangent;
		SDataStreamUnit UV0;
		SDataStreamUnit UV1;
		SDataStreamUnit UV2;
		SDataStreamUnit UV3;
		SDataStreamUnit Color;

		bool HasNormal()  const { return Normal.bValid; }
		bool HasTangent()  const { return Tangent.bValid; }
		bool HasUV0()      const { return UV0.bValid; }
		bool HasUV1()      const { return UV1.bValid; }
		bool HasUV2()      const { return UV2.bValid; }
		bool HasUV3()      const { return UV3.bValid; }
		bool HasColor()    const { return Color.bValid; }
	};

	//struct SVertexStreamUnit
	//{
	//	ID3D12Resource* VertexBuffer = nullptr;
	//	uint32 Stride = 0;
	//	uint32 Offset = 0;
	//	uint32 Slot = 0;  // input slot
	//	EVertexElementType Type = EVertexElementType::Float3;
	//	D3D12_INPUT_CLASSIFICATION Classification = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	//	uint32 InstanceStepRate = 0;

	//	DXGI_FORMAT GetDXGIFormat() const
	//	{
	//		switch (Type)
	//		{
	//		case EVertexElementType::Float1:  return DXGI_FORMAT_R32_FLOAT;
	//		case EVertexElementType::Float2:  return DXGI_FORMAT_R32G32_FLOAT;
	//		case EVertexElementType::Float3:  return DXGI_FORMAT_R32G32B32_FLOAT;
	//		case EVertexElementType::Float4:  return DXGI_FORMAT_R32G32B32A32_FLOAT;
	//		case EVertexElementType::Uint4:   return DXGI_FORMAT_R32G32B32A32_UINT;
	//		case EVertexElementType::Ushort4: return DXGI_FORMAT_R16G16B16A16_UINT;
	//		case EVertexElementType::Ubyte4:  return DXGI_FORMAT_R8G8B8A8_UNORM;
	//	//	case EVertexElementType::PackedNormal: return DXGI_FORMAT_R10G10B10A2_UNORM;
	//		default: return DXGI_FORMAT_UNKNOWN;
	//		}
	//	}
	//};

	struct SInputElement
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
		
		// Éú³É InputLayout
		virtual TArray<D3D12_INPUT_ELEMENT_DESC> BuildInputLayout(const std::vector<std::string>& elements ) = 0;

		// shader ºê£¨UE µÄ ModifyCompilationEnvironment£©
		virtual void FillShaderDefines(std::unordered_map<std::string, std::string>& defines) {}
	};

	class CStaticVertexFactory : public CVertexStreamFactory
	{
	public:
		CStaticVertexFactory();
		virtual ~CStaticVertexFactory();

		virtual TArray<D3D12_INPUT_ELEMENT_DESC> BuildInputLayout(const std::vector<std::string>& elements) override;

	public:
		SMeshVertexSchema Schema;
		uint32 NumVertices = 0;
		TArray<D3D12_VERTEX_BUFFER_VIEW> mCachedVerticesViews;
	};
}