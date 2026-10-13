#include "StaticMesh.h"
#include "MeshAttributes.h"
#include "VertexStreamFactory.h"
#include "Material.h"
#include "MaterialMgr.h"
#include "MLEngine.h"
namespace ML
{
	CStaticMesh::CStaticMesh()
	{
	}

	CStaticMesh::~CStaticMesh()
	{
		for (int32 i = 0; i < SourceModels.Num(); ++i)
		{
			delete SourceModels[i];
		}
		SourceModels.Empty();
	}

	int32 CStaticMesh::AddSourceModel()
	{
		CStaticMeshDescription* Model = new CStaticMeshDescription();
		return SourceModels.Add(Model);
	}

	CStaticMeshDescription* CStaticMesh::GetSourceModel(int32 LODIndex)
	{
		if (LODIndex < 0 || LODIndex >= SourceModels.Num())
		{
			return nullptr;
		}
		return SourceModels[LODIndex];
	}

	void CStaticMesh::Build()
	{
		for (int32 i = 0; i < RenderData.Num(); ++i)
		{
			delete RenderData[i];
		}
		RenderData.Empty();
		for (int32 i = 0; i < SourceModels.Num(); ++i)
		{
			CStaticMeshDescription* SrcModel = SourceModels[i];
			if (nullptr == SrcModel)
			{
				continue;
			}

			CMeshAttributes Attrs;
			Attrs.BakeFromDescription(SrcModel->GetMeshDescription());
			if (!Attrs.IsValid())
			{
				continue;
			}

			CStaticMeshLODResources* LOD = new CStaticMeshLODResources();
			LOD->InitResources(Attrs);
			RenderData.Add(LOD);
		}
	}

	CStaticVertexFactory* CStaticMesh::GetVertexFactory(int32 LODIndex) const
	{
		if (LODIndex < 0 || LODIndex >= RenderData.Num())
		{
			return nullptr;
		}
		return RenderData[LODIndex]->VertexFactory;
	}

	CStaticMeshLODResources* CStaticMesh::GetRenderData(int32 LODIndex) const
	{
		if (LODIndex < 0 || LODIndex >= RenderData.Num())
		{
			return nullptr;
		}
		return RenderData[LODIndex];
	}

	int32 CStaticMesh::AddMaterialSlot(const std::string& Name)
	{
		FStaticMaterialSlot Slot;
		Slot.Name = Name;
		Slot.Material = GEngine ? GEngine->GetMgr<CMaterialMgr>()->GetDefaultMaterial() : nullptr;
		return MaterialSlots.Add(Slot);
	}

	CMaterial* CStaticMesh::GetMaterialForSection(int32 SectionIndex) const
	{
		CMaterialMgr* MatMgr = GEngine ? GEngine->GetMgr<CMaterialMgr>() : nullptr;
		CMaterial* Fallback = MatMgr ? MatMgr->GetDefaultMaterial() : nullptr;
		if (SectionIndex < 0 || SectionIndex >= MaterialSlots.Num())
		{
			return Fallback;
		}
		CMaterial* M = MaterialSlots[SectionIndex].Material;
		return M ? M : Fallback;
	}
}