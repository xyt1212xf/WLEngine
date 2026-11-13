#include "LandscapeComponent.h"
#include "LandscapeLODResources.h"
#include "LandscapeVertexFactory.h"
#include "Scene.h"
#include "Material.h"

namespace ML
{
	CLandscapeComponent::CLandscapeComponent()
	{
		LOD0 = new CLandscapeLODResources();
		VF = new CLandscapeVertexFactory(); 
	}

	CLandscapeComponent::~CLandscapeComponent()
	{
		SafeDelete(LOD0);
		SafeDelete(VF);
	}
	void CLandscapeComponent::BuildRenderData()
	{
		const uint32 NumLayers = LandscapeMaterial ? LandscapeMaterial->GetLayerCount() : 1;
		LOD0->Build(Heightfield, 0, VF, NumLayers);
	}
	CPrimitiveSceneProxy* CLandscapeComponent::CreateSceneProxy()
	{
		if (nullptr == LOD0 || nullptr == VF || nullptr == LandscapeMaterial)
		{
			return nullptr;
		}
		RenderProxyId = AllocateSceneProxyId();
		return new CPrimitiveSceneProxy(RenderProxyId,
			(CMesh*)nullptr, (CMeshLODResources*)LOD0, (CVertexStreamFactory*)VF,
			LandscapeMaterial, GetWorldMatrix(), 0, IsVisible());
	}
}
