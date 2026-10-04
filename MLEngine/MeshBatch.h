#pragma once
#include "NumericLimits.h"
#include "Matrix.h"
namespace ML
{
	class CStaticMesh;
	class CStaticVertexFactory;
	class CStaticMeshLODResources;

	struct FMeshBatchElement
	{
		uint32 FirstIndex = 0;
		uint32 NumIndices = 0;
		int32  BaseVertex = 0;
		uint32 InstanceCount = 1;
	};

	struct FMeshBatch
	{
		CStaticMesh* Mesh = nullptr;
		CStaticMeshLODResources* LOD = nullptr;
		CStaticVertexFactory* VertexFactory = nullptr;

		FMatrix44	WorldMatrix;
		uint32		SectionIndex = 0;

		FMeshBatchElement Element;
	};
}