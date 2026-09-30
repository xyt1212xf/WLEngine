#pragma once
#include "Mesh.h"
#include "Array.h"
#include "StaticMeshDescription.h"
#include "StaticMeshResources.h"
namespace ML
{
	class CStaticMesh : public CMesh
	{
	public:
		CStaticMesh();
		virtual ~CStaticMesh();

		int32 AddSourceModel();
		CStaticMeshDescription* GetSourceModel(int32 LODIndex);
		int32 GetNumSourceModels() const { return SourceModels.Num(); }
		void Build();

		// Baked render data per LOD (null until Build() has been called).
		CStaticMeshLODResources* GetRenderData(int32 LODIndex) const;
		int32 GetNumLODs() const { return RenderData.Num(); }

		int32 AddMaterialSlot(const std::string& Name);
		int32 GetNumMaterialSlots() const { return MaterialSlots.Num(); }
		const std::string& GetMaterialSlotName(int32 Index) const { return MaterialSlots[Index]; }
	
	private:
		TArray<CStaticMeshDescription*> SourceModels;
		TArray<CStaticMeshLODResources*> RenderData; 
		TArray<std::string> MaterialSlots;
	};
}