#include "StaticMeshResources.h"
#include "MathLib.h"
#include "VertexStreamFactory.h"
#include "RHIbuffer.h"
#include "GraphicPlug.h"
#include "MLEngine.h"

namespace ML
{
	CStaticMeshLODResources::~CStaticMeshLODResources()
	{
		ReleaseResources();
	}

	void CStaticMeshLODResources::InitResources(const CMeshAttributes& Attrs)
	{
		ReleaseResources();

		// Each LOD owns its own CStaticVertexFactory (owner semantics; deleted in ReleaseResources).
		// Decoupled from CGraphicPlug's default VF - mesh layout is fully independent.
		VertexFactory = new CStaticVertexFactory();

		if (!Attrs.IsValid())
		{
			return;
		}

		const std::vector<FStaticMeshSection>& SrcSections = Attrs.GetSections();
		for (int32 i = 0; i < SrcSections.size(); ++i)
		{
			Sections.push_back(SrcSections[i]);
		}
		NumVertices = Attrs.GetNumVertices();
		NumIndices = Attrs.GetNumIndices();

		const std::vector<Vec3F>& Positions = Attrs.GetPositions();
		if (Positions.size() > 0)
		{
			Vec3F Min = Positions[0];
			Vec3F Max = Positions[0];
			for (int32 i = 1; i < Positions.size(); ++i)
			{
				const Vec3F& P = Positions[i];
				Min.x = (P.x < Min.x) ? P.x : Min.x;
				Min.y = (P.y < Min.y) ? P.y : Min.y;
				Min.z = (P.z < Min.z) ? P.z : Min.z;
				Max.x = (P.x > Max.x) ? P.x : Max.x;
				Max.y = (P.y > Max.y) ? P.y : Max.y;
				Max.z = (P.z > Max.z) ? P.z : Max.z;
			}

			BoundsOrigin = Vec3F((Min.x + Max.x) * 0.5f,
				(Min.y + Max.y) * 0.5f,
				(Min.z + Max.z) * 0.5f);

			float RadiusSq = 0.0f;
			for (int32 i = 0; i < Positions.size(); ++i)
			{
				const Vec3F& P = Positions[i];
				const float dx = P.x - BoundsOrigin.x;
				const float dy = P.y - BoundsOrigin.y;
				const float dz = P.z - BoundsOrigin.z;
				const float DistSq = dx * dx + dy * dy + dz * dz;
				if (DistSq > RadiusSq)
				{
					RadiusSq = DistSq;
				}
			}
			BoundsRadius = sqrtf(RadiusSq);

			VertexBuffers.Position = new CRHIBuffer();
			VertexBuffers.Position->Create(
				EBufferType::Vertex,
				(UINT32)Positions.size() * sizeof(Vec3F),
				sizeof(Vec3F),
				Positions.data()
			);

		}
		const std::vector<uint32>& Indices = Attrs.GetIndices();
		if (Indices.size() > 0)
		{
			IndexBuffer = new CRHIBuffer();
			IndexBuffer->Create(
				EBufferType::Index,
				(UINT32)Indices.size() * sizeof(uint32),
				sizeof(uint32),
				Indices.data()
			);
		}

		// 如果有法线
		const std::vector<Vec3F>& Normals = Attrs.GetNormals();
		if (Normals.size() > 0)
		{
			VertexBuffers.Normal = new CRHIBuffer();
			VertexBuffers.Normal->Create(
				EBufferType::Vertex,
				(UINT32)Normals.size() * sizeof(Vec3F),
				sizeof(Vec3F),
				Normals.data()
			);
		}
		// 如果有UV
		const std::vector<Vec2F>& UV = Attrs.GetUVs();
		if (UV.size() > 0)
		{
			VertexBuffers.UV = new CRHIBuffer();
			VertexBuffers.UV->Create(
				EBufferType::Vertex,
				(UINT32)UV.size() * sizeof(Vec2F),
				sizeof(Vec2F),
				UV.data()
			);
		}

		// 如果有切线
		const std::vector<Vec4F>& Tangents = Attrs.GetTangents();
		if (Tangents.size() > 0)
		{
			VertexBuffers.Tangent = new CRHIBuffer();
			VertexBuffers.Tangent->Create(
				EBufferType::Vertex,
				(UINT32)Tangents.size() * sizeof(Vec4F),
				sizeof(Vec4F),
				Tangents.data()
			);
		}

		// 如果有颜色
		const std::vector<Vec4F>& Colors = Attrs.GetColors();
		if (Colors.size() > 0)
		{
			VertexBuffers.Color = new CRHIBuffer();
			VertexBuffers.Color->Create(
				EBufferType::Vertex,
				(UINT32)Colors.size() * sizeof(Vec4F),
				sizeof(Vec4F),
				Colors.data()
			);
		}

		// 填充顶点工厂的流布局(SDataStreamUnit), 再构建 InputLayout。
		// BuildInputLayout 会同时缓存 VB 视图(mCachedVerticesViews),
		// 渲染时 IASetVertexBuffers 直接用。
		if (VertexFactory)
		{
			uint32 NextSlot = 0;
			VertexFactory->NumVertices = NumVertices;
			auto FillStream = [](FDataStreamUnit& Unit, ID3D12Resource* Buffer, uint32 Slot, uint32 Stride, DXGI_FORMAT Format)
				{
					Unit.GpuBuffer	= Buffer;
					Unit.Slot		= Slot;
					Unit.Offset		= 0;
					Unit.Stride		= Stride;
					Unit.Format		= Format;
					Unit.bValid		= (Buffer != nullptr);
				};
			std::vector<std::string> Elements;
			if (VertexBuffers.Position)
			{
				FillStream(VertexFactory->Schema.Position,
					VertexBuffers.Position ? VertexBuffers.Position->GetResource() : nullptr,
					NextSlot++, sizeof(Vec3F), DXGI_FORMAT_R32G32B32_FLOAT);
				Elements.push_back("POSITION");
			}
			if (VertexBuffers.Normal)
			{
				FillStream(VertexFactory->Schema.Normal,
					VertexBuffers.Normal ? VertexBuffers.Normal->GetResource() : nullptr,
					NextSlot++, sizeof(Vec3F), DXGI_FORMAT_R32G32B32_FLOAT);
				Elements.push_back("NORMAL");
			}
			if (VertexBuffers.Color)
			{
				FillStream(VertexFactory->Schema.Color,
					VertexBuffers.Color ? VertexBuffers.Color->GetResource() : nullptr,
					NextSlot++, sizeof(Vec4F), DXGI_FORMAT_R32G32B32A32_FLOAT);
				Elements.push_back("COLOR");
			}
			if (VertexBuffers.UV)
			{
				FillStream(VertexFactory->Schema.UV0,
					VertexBuffers.UV ? VertexBuffers.UV->GetResource() : nullptr,
					NextSlot++, sizeof(Vec2F), DXGI_FORMAT_R32G32_FLOAT);
				Elements.push_back("TEXCOORD");
			}
			if (VertexBuffers.Tangent)
			{
				FillStream(VertexFactory->Schema.Tangent,
					VertexBuffers.Tangent ? VertexBuffers.Tangent->GetResource() : nullptr,
					NextSlot++, sizeof(Vec4F), DXGI_FORMAT_R32G32B32A32_FLOAT);
				Elements.push_back("TANGENT");
			}
			
			if (!Elements.empty())
			{
				VertexFactory->BuildInputLayout(Elements);
			}
		}
	}

	void CStaticMeshLODResources::ReleaseResources()
	{
		VertexBuffers.Release();
		SafeDelete(IndexBuffer);
		Sections.clear();
		NumVertices = 0;
		NumIndices = 0;
		BoundsOrigin = Vec3F(0.0f, 0.0f, 0.0f);
		BoundsRadius = 0.0f;

		SafeDelete(VertexFactory);
	}


}
