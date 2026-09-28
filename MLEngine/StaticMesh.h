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
		void BuildFromStaticMeshDescriptions(const TArray<CStaticMeshDescription*>& StaticMeshDescriptions);
	};
}