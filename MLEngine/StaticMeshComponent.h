#pragma once
#include "Component.h"
namespace ML
{
	class CStaticMesh;
	class CStaticMeshComponent : public CComponent
	{
	public:
		CStaticMeshComponent();
		virtual ~CStaticMeshComponent();

	protected:
		CStaticMesh* Mesh = nullptr;
	};
}