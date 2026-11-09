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
		void BuildRenderData();
		FLandscapeHeightfield& GetHeightfield() { return Heightfield; }
		void SetLandscapeMaterial(CMaterial* InMat) { LandscapeMaterial = InMat; }
		CMaterial* GetLandscapeMaterial() const { return LandscapeMaterial; }
		CLandscapeLODResources* GetLOD(int32 i) const { return (i == 0) ? LOD0 : nullptr; }
		CLandscapeVertexFactory* GetVertexFactory() const { return VF; }

	private:
		FPrimitiveSceneProxy* CreateSceneProxy();

	private:
		FLandscapeHeightfield   Heightfield;
		CLandscapeLODResources* LOD0 = nullptr;
		CLandscapeVertexFactory* VF = nullptr;
		CMaterial* LandscapeMaterial = nullptr;
	};
}
