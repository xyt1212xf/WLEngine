#include "StaticMeshComponent.h"
#include "StaticMeshResources.h"
#include "ViewMatrices.h"
#include "MeshBatch.h"
#include "StaticMesh.h"
namespace ML
{

	CStaticMeshComponent::CStaticMeshComponent()
	{

	}

	CStaticMeshComponent::~CStaticMeshComponent()
	{

	}

	void CStaticMeshComponent::GetMeshBatch(FMeshBatch& OutBatch, const FMatrix44& InViewProj) const
	{
		if (!StaticMesh || !bVisible)
		{
			return;
		}
		CStaticMeshLODResources* LOD = StaticMesh->GetRenderData(0);
		const FStaticMeshSection& Section = LOD->Sections[0];

		OutBatch.Mesh = StaticMesh;
		OutBatch.LOD = LOD;
		OutBatch.VertexFactory = StaticMesh->GetVertexFactory(0);
		OutBatch.WorldMatrix = GetWorldMatrix();
		OutBatch.SectionIndex = 0;
		OutBatch.Element.FirstIndex = Section.FirstIndex;
		OutBatch.Element.NumIndices = Section.NumTriangles * 3;
	}

	FMatrix44 CStaticMeshComponent::GetWorldMatrix() const
	{
		// 构造 TRS 矩阵
		FMatrix44 TranslationMatrix = FMatrix44::MakeTranslation(Location);
		FMatrix44 RotationMatrix = FMatrix44::MakeRotation(Rotation);
		FMatrix44 ScaleMatrix = FMatrix44::MakeScale(Scale);

		// 世界矩阵 = Scale * Rotation * Translation（注意顺序）
		return ScaleMatrix * RotationMatrix * TranslationMatrix;
	}

	void CStaticMeshComponent::SetLocation(const Vec3F& InLocation)
	{
		Location = InLocation;
	}

	void CStaticMeshComponent::SetRotation(const CQuaternion& InRotation)
	{
		Rotation = InRotation;
	}

	void CStaticMeshComponent::SetScale(const Vec3F& InScale)
	{
		Scale = InScale;
	}

}