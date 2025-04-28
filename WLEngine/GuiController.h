#pragma once
#include "Controller.h"

namespace WL
{
	class  CGuiController : public CController
	{
	public:
		CGuiController();

	protected:
		virtual bool onEvent(event& e) final;
		
	};
}