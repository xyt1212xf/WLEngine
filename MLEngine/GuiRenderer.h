#pragma once
#include "Renderer.h"
#include "GuiEntity.h"
namespace WL
{
	class CGuiRenderer : public CRenderer
	{
		typedef CRenderer Parent;
	public:
		CGuiRenderer();
		virtual ~CGuiRenderer();
	
	private:
		virtual void drawEntity(CEntity* pEntitiy) final;
		//virtual void addEntity(CEntity* pEntitiy)final;
	};
}