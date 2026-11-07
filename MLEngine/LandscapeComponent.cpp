#include "LandscapeComponent.h"
#include "LandscapeLODResources.h"
#include "LandscapeVertexFactory.h"
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

}
