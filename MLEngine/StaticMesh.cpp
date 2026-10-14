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
		for (int32 i = 0; i < SourceModels.size(); ++i)
		{
			delete SourceModels[i];
		}
		SourceModels.clear();
	}

	int32 CStaticMesh::AddSourceModel()
	{
		CStaticMeshDescription* Model = new CStaticMeshDescription();
		SourceModels.push_back(Model);
		return (int32)SourceModels.size()-1;
	}

	CStaticMeshDescription* CStaticMesh::GetSourceModel(int32 LODIndex)
	{
		if (LODIndex < 0 || LODIndex >= SourceModels.size())
		{
			return nullptr;
		}
		return SourceModels[LODIndex];
	}

	void CStaticMesh::Build()
	{
		for (int32 i = 0; i < RenderData.size(); ++i)
		{
			delete RenderData[i];
		}
		RenderData.clear();
		for (int32 i = 0; i < SourceModels.size(); ++i)
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
			RenderData.push_back(LOD);
		}
	}

	CStaticVertexFactory* CStaticMesh::GetVertexFactory(int32 LODIndex) const
	{
		if (LODIndex < 0 || LODIndex >= RenderData.size())
		{
			return nullptr;
		}
		return RenderData[LODIndex]->VertexFactory;
	}

	CStaticMeshLODResources* CStaticMesh::GetRenderData(int32 LODIndex) const
	{
		if (LODIndex < 0 || LODIndex >= RenderData.size())
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
		MaterialSlots.push_back(Slot);
		return MaterialSlots.size();
	}

	CMaterial* CStaticMesh::GetMaterialForSection(int32 SectionIndex) const
	{
		CMaterialMgr* MatMgr = GEngine ? GEngine->GetMgr<CMaterialMgr>() : nullptr;
		CMaterial* Fallback = MatMgr ? MatMgr->GetDefaultMaterial() : nullptr;
		if (SectionIndex < 0 || SectionIndex >= MaterialSlots.size())
		{
			return Fallback;
		}
		CMaterial* M = MaterialSlots[SectionIndex].Material;
		return M ? M : Fallback;
	}
}
