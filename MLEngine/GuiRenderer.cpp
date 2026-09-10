#include "GuiRenderer.h"
namespace WL
{
	CGuiRenderer::CGuiRenderer()
	{
		mOrder = GuiRenderer;
	}

	CGuiRenderer::~CGuiRenderer()
	{

	}

	void CGuiRenderer::drawEntity(CEntity* pEntitiy)
	{

	}

	//void CGuiRenderer::addEntity(CEntity* pEntitiy)
	//{
		//auto pGuiEntity = dynamic_cast<CGuiEntity*>(pEntitiy);
		//if (nullptr != pGuiEntity)
		//{
		//	Super::addEntity(pEntitiy);
		//}
	//}
}