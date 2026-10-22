#pragma once
#include "StreamableRenderAsset.h"
#include "RHIbuffer.h"
namespace ML
{
	class CMeshAttributes;

	// UE: UStaticMesh : UStreamableRenderAsset : UObject.
	// Meshes stream per-LOD, so they take the streaming facet; materials do not.
	class MLENGINE_API CMesh : public CStreamableRenderAsset
	{
	public:
		CMesh();
		virtual ~CMesh();	
	//	virtual void BuildFromMeshDescriptions(const CMeshAttributes& Attrs) = 0;

	protected:
		CRHIBuffer* VertexBuffer = nullptr;
		CRHIBuffer* IndexBuffer = nullptr;
	};
}
