#pragma once
#include "Mesh.h"
#include "StaticMeshDescription.h"
#include "StaticMeshResources.h"
namespace ML
{
	class CStaticVertexFactory;
	class CMaterial;

	struct FStaticMaterialSlot
	{
		std::string	Name;
		CMaterial* Material = nullptr;	// engine owns lifetime, slot is just a reference
	};


	class MLENGINE_API CStaticMesh : public CMesh
	{
	public:
		CStaticMesh();
		virtual ~CStaticMesh();

		int32 AddSourceModel();
		CStaticMeshDescription* GetSourceModel(int32 LODIndex);
		int32 GetNumSourceModels() const { return (int32)SourceModels.size(); }
		void Build();

		// Baked render data per LOD (null until Build() has been called).
		CStaticVertexFactory* GetVertexFactory(int32 LODIndex) const;
		CStaticMeshLODResources* GetRenderData(int32 LODIndex) const;
		int32 GetNumLODs() const { return RenderData.size(); }

		int32 AddMaterialSlot(const std::string& Name);
		int32 GetNumMaterialSlots() const { return (int32)MaterialSlots.size(); }
		// Returns the slot; caller may reassign the material via .Material.
		FStaticMaterialSlot& GetMaterialSlot(int32 Index) { return MaterialSlots[Index]; }
		const FStaticMaterialSlot& GetMaterialSlot(int32 Index) const { return MaterialSlots[Index]; }
		// Final material for a section (falls back to DefaultMaterial when unassigned).
		CMaterial* GetMaterialForSection(int32 SectionIndex) const;

	private:
		std::vector<CStaticMeshDescription*> SourceModels;
		std::vector<CStaticMeshLODResources*> RenderData;
		std::vector<FStaticMaterialSlot> MaterialSlots;
	};
}
