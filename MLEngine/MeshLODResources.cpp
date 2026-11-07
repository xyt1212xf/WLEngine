#include "MeshLODResources.h"
namespace ML
{

	CMeshLODResources::CMeshLODResources()
	{

	}

	CMeshLODResources::~CMeshLODResources()
	{
		VertexBuffers.Release();
		SafeDelete(IndexBuffer);
	}

}
