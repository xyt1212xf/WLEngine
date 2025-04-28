#pragma once
#include "Message.h"
#include "Controller.h"
namespace WL
{
	class CGameController : public CController
	{
	public:
		CGameController();
		
	protected:
		virtual bool onEvent(event& e) final;
	};
}