#include "MeshLODResources.h"
namespace ML
{

	CMeshLODResources::~CMeshLODResources()
	{
		VertexBuffers.Release();
		SafeDelete(IndexBuffer);
	}

}
