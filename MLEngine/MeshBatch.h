#pragma once
#include "NumericLimits.h"
#include "Matrix.h"
namespace ML
{
	class CStaticMesh;
	class CStaticVertexFactory;
	class CStaticMeshLODResources;
	class CMaterial;

	struct FMeshBatchElement
	{
		uint32 FirstIndex = 0;
		uint32 NumIndices = 0;
		int32  BaseVertex = 0;
		uint32 InstanceCount = 1;
	};

	// UE counterpart: FMeshBatch (Engine/Public/MeshBatch.h).
	// P0 simplification: carries a raw CMaterial* directly, no FMaterialRenderProxy.
	// When per-draw parameter overrides are needed (fade / dither / instance custom data),
	// swap CMaterial* for CMaterialRenderProxy*; the renderer interface stays unchanged.
	struct FMeshBatch
	{
		CStaticMesh* Mesh = nullptr;
		CStaticMeshLODResources* LOD = nullptr;
		CStaticVertexFactory* VertexFactory = nullptr;
		CMaterial* Material = nullptr;  // resolved from Section by Component::GetMeshBatch

		FMatrix44	WorldMatrix;
		uint32		SectionIndex = 0;

		FMeshBatchElement Element;
	};
}