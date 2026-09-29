#pragma once
//#include "StreamableRenderAsset.h"
#include "Resource.h"
#include "RHIbuffer.h"
namespace ML
{
	class CMeshAttributes;
	class CMesh : public CResource
	{
	public:
		CMesh();
		virtual ~CMesh();	
		virtual void BuildFromMeshDescriptions(const CMeshAttributes& Attrs) = 0;

	protected:
		CRHIBuffer* VertexBuffer = nullptr;
		CRHIBuffer* IndexBuffer = nullptr;
	};
}