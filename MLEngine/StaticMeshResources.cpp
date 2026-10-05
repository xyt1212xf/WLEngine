#include "StaticMeshResources.h"
#include "MathLib.h"
#include "VertexStreamFactory.h"
#include "DX12RHIbuffer.h"


namespace ML
{
	CStaticMeshLODResources::~CStaticMeshLODResources()
	{
		ReleaseResources();
	}

	void CStaticMeshLODResources::InitResources(const CMeshAttributes& Attrs)
	{
		ReleaseResources();

		VertexFactory = new CStaticVertexFactory();
	//	VertexFactory->BuildInputLayout();

		if (!Attrs.IsValid())
		{
			return;
		}

		const TArray<FStaticMeshSection>& SrcSections = Attrs.GetSections();
		for (int32 i = 0; i < SrcSections.Num(); ++i)
		{
			Sections.Add(SrcSections[i]);
		}
		NumVertices = Attrs.GetNumVertices();
		NumIndices = Attrs.GetNumIndices();

		const TArray<Vec3F>& Positions = Attrs.GetPositions();
		if (Positions.Num() > 0)
		{
			Vec3F Min = Positions[0];
			Vec3F Max = Positions[0];
			for (int32 i = 1; i < Positions.Num(); ++i)
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
			for (int32 i = 0; i < Positions.Num(); ++i)
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

			PositionBuffer = new CRHIBuffer();
			PositionBuffer->Create(
				EBufferType::Vertex,
				Positions.Num() * sizeof(Vec3F),
				sizeof(Vec3F),
				Positions.GetData()
			);

		}
		const TArray<uint32>& Indices = Attrs.GetIndices();
		if (Indices.Num() > 0)
		{
			IndexBuffer = new CRHIBuffer();
			IndexBuffer->Create(
				EBufferType::Index,
				Indices.Num() * sizeof(uint32),
				sizeof(uint32),
				Indices.GetData()
			);
		}

		// 如果有切线
		const TArray<Vec4F>& Tangents = Attrs.GetTangents();
		if (Tangents.Num() > 0)
		{
			TangentBuffer = new CRHIBuffer();
			TangentBuffer->Create(
				EBufferType::Vertex,
				Tangents.Num() * sizeof(Vec4F),
				sizeof(Vec4F),
				Tangents.GetData()
			);
		}

		// 如果有颜色
		const TArray<Vec4F>& Colors = Attrs.GetColors();
		if (Colors.Num() > 0)
		{
			ColorBuffer = new CRHIBuffer();
			ColorBuffer->Create(
				EBufferType::Vertex,
				Colors.Num() * sizeof(Vec4F),
				sizeof(Vec4F),
				Colors.GetData()
			);
		}
	}

	void CStaticMeshLODResources::ReleaseResources()
	{
		PositionBuffer = nullptr;
		TangentBuffer = nullptr;
		ColorBuffer = nullptr;
		IndexBuffer = nullptr;

		Sections.Empty();
		NumVertices = 0;
		NumIndices = 0;
		BoundsOrigin = Vec3F(0.0f, 0.0f, 0.0f);
		BoundsRadius = 0.0f;

		SafeDelete(VertexFactory);
	}


}