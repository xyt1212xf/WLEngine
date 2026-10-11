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
}