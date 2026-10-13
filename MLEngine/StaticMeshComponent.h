#pragma once
#include "MeshComponent.h"
#include "Matrix.h"
#include "Quaternion.h"
#include "Vector.h"
namespace ML
{
	class CStaticMesh;
	struct FMeshBatch;
	class MLENGINE_API CStaticMeshComponent : public CMeshComponent
	{
	public:
		CStaticMeshComponent();
		virtual ~CStaticMeshComponent();
		void GetMeshBatch(FMeshBatch& OutBatch,const FMatrix44& InViewProj) const;
		void SetStaticMesh(CStaticMesh* InMesh) { StaticMesh = InMesh; }
		
	protected:
		CStaticMesh* StaticMesh = nullptr;
	};
}
