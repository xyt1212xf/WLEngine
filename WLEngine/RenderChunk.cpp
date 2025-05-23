#include "RenderChunk.h"
namespace WL
{

	void SRenderUnit::reset()
	{
		//pWorldMT = nullptr;
		//pViewMT = nullptr;
		//pProjMT = nullptr;
		pMaterialInstance = nullptr;
		geometry.clear();
	}

}