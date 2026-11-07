#include "LandscapeLODResources.h"
#include "LandscapeHeightfield.h"
namespace ML
{
	namespace
	{
		float HF_HeightClamped(const FLandscapeHeightfield& HF, int32 x, int32 y)
		{
			x = (x < 0) ? 0 : (x > HF.ComponentSizeQuads ? HF.ComponentSizeQuads : x);
			y = (y < 0) ? 0 : (y > HF.ComponentSizeQuads ? HF.ComponentSizeQuads : y);
			return HF.GetHeight(x, y) * HF.ZScale;
		}
	}


	CLandscapeLODResources::~CLandscapeLODResources()
	{
		Release();
	}

	void CLandscapeLODResources::Build(const FLandscapeHeightfield& HF, int32 LODIndex, CLandscapeVertexFactory* InVF)
	{

	}

	void CLandscapeLODResources::Release()
	{
		VertexBuffers.Release();
		SafeDelete(IndexBuffer);
		NumVertices = 0;
		NumIndices = 0;
	}

}
