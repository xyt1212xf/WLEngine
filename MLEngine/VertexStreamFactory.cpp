#include "VertexStreamFactory.h"
namespace ML
{

	CVertexStreamFactory::CVertexStreamFactory()
	{

	}

	CVertexStreamFactory::~CVertexStreamFactory()
	{

	}

	CStaticVertexFactory::CStaticVertexFactory()
	{

	}

	CStaticVertexFactory::~CStaticVertexFactory()
	{

	}

	TArray<D3D12_INPUT_ELEMENT_DESC> CStaticVertexFactory::BuildInputLayout(const std::vector<std::string>& elements)
	{
		mCachedVerticesViews.Empty();
		TArray<D3D12_INPUT_ELEMENT_DESC> elems;

		if (elements.size() != 0)
		{
			uint32 ai = 0;
			auto Add = [&](const char* sem, uint32 idx, SDataStreamUnit& c)
				{
					c.bValid = true;
					D3D12_INPUT_ELEMENT_DESC e = {};
					e.SemanticName = sem;
					e.SemanticIndex = idx;
					e.Format = c.Format;
					e.InputSlot = c.Slot;
					e.AlignedByteOffset = c.Offset;
					e.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
					e.InstanceDataStepRate = 0;
					elems.Add(e);

					D3D12_VERTEX_BUFFER_VIEW v = {};
					v.BufferLocation = c.Buffer->GetGPUVirtualAddress() + c.Offset;
					v.SizeInBytes = c.Stride * NumVertices;
					v.StrideInBytes = c.Stride;
					mCachedVerticesViews.Add(v);
					
					ai++;
				};

			for (const std::string& name : elements)
			{
				if ("POSITION" == name)
				{
					Add("POSITION", 0, Schema.Position);
				}
				else if ("NORMAL" == name)
				{
					Add("NORMAL", 0, Schema.Normal);
				}
				else if ("TANGENT" == name)
				{
					Add("TANGENT", 0, Schema.Tangent);
				}
				else if ("TEXCOORD" == name ||"TEXCOORD0" == name)
				{
					Add("TEXCOORD", 0, Schema.UV0);
				}
				else if ("TEXCOORD1" == name)
				{
					Add("TEXCOORD", 1, Schema.UV1);
				}
				else if ("TEXCOORD2" == name)
				{
					Add("TEXCOORD", 2, Schema.UV2);
				}
				else if ("TEXCOORD3" == name)
				{
					Add("TEXCOORD", 3, Schema.UV3);
				}
				else if ("COLOR" == name)
				{
					Add("COLOR", 0, Schema.Color);
				}
			}
		}
		return elems;
	}
}