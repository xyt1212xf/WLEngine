#pragma once
#include "MeshLODResources.h"
namespace ML
{
	class CLandscapeVertexFactory;
	struct FLandscapeHeightfield;

	class CLandscapeLODResources : public CMeshLODResources
    {
    public:
        CLandscapeLODResources() = default;
        ~CLandscapeLODResources();
		void Build(const FLandscapeHeightfield& HF, int32 LODIndex, CLandscapeVertexFactory* InVF);
		void Release();
	private:
		
    };
}
