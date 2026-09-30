#include "StaticMeshResources.h"
#include "MathLib.h"

namespace ML
{
	CStaticMeshLODResources::~CStaticMeshLODResources()
	{
		ReleaseResources();
	}

	void CStaticMeshLODResources::InitResources(const CMeshAttributes& Attrs)
	{
		ReleaseResources();

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
	}
}