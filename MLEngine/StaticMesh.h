#pragma once
#include "Mesh.h"
#include "Array.h"
#include "StaticMeshDescription.h"
namespace ML
{
	class CStaticMesh : public CMesh
	{
	public:
		CStaticMesh();
		virtual ~CStaticMesh();
		virtual void BuildFromMeshDescriptions(const CMeshAttributes& Attrs);
		
	};
}