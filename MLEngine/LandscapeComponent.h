#pragma once
#include "PrimitiveComponent.h"
#include "LandscapeHeightfield.h"

namespace ML
{
	class CLandscapeLODResources;
	class CMaterial;
	class CLandscapeVertexFactory;

	class CLandscapeComponent : public CPrimitiveComponent
	{
	public:
		CLandscapeComponent();
		virtual ~CLandscapeComponent();

	private:
		FLandscapeHeightfield   Heightfield;
		CLandscapeLODResources* LOD0 = nullptr;
		CLandscapeVertexFactory* VF = nullptr;
		CMaterial* LandscapeMaterial = nullptr;
	};
}
