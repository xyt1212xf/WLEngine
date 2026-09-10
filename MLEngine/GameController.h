#pragma once
#include "Message.h"
#include "Controller.h"
namespace WL
{
	class CGameController : public SController
	{
	public:
		CGameController();
		
	protected:
		virtual bool onEvent(SEvent& e) final;
	};
}