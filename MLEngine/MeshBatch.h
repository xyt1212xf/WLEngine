#pragma once
#include "NumericLimits.h"
#include "Matrix.h"
namespace ML
{
	class CMesh;
	class CVertexStreamFactory;
	class CMeshLODResources;
	class CMaterial;

	struct FMeshBatchElement
	{
		uint32 FirstIndex = 0;
		uint32 NumIndices = 0;
		int32  BaseVertex = 0;
		uint32 InstanceCount = 1;
	};

	enum class EMeshPass : uint8
	{
		BasePass = 0,   // 不透明（含 Masked）
		SkyPass,        // 天空 mesh（材质 IsSky）
		Translucent,    // 半透明
	};

	// UE counterpart: FMeshBatch (Engine/Public/MeshBatch.h).
	// P0 simplification: carries a raw CMaterial* directly, no FMaterialRenderProxy.
	// When per-draw parameter overrides are needed (fade / dither / instance custom data),
	// swap CMaterial* for CMaterialRenderProxy*; the renderer interface stays unchanged.
	struct FMeshBatch
	{
		CMesh* Mesh = nullptr;
		CMeshLODResources* LOD = nullptr;
		CVertexStreamFactory* VertexFactory = nullptr;
		CMaterial* Material = nullptr;  // 由 FPrimitiveSceneProxy 构造时解析（override 优先）

		FMatrix44	WorldMatrix;
		uint32		SectionIndex = 0;

		FMeshBatchElement Element;
	};
}
