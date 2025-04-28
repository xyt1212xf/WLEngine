#include "RenderChunk.h"
namespace WL
{

	void RenderUnit::reset()
	{
		//pWorldMT = nullptr;
		//pViewMT = nullptr;
		//pProjMT = nullptr;
		pMaterialInstance = nullptr;
		geometry.clear();
	}

}