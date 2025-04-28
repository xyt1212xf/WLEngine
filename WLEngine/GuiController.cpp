#include "pch.h"
#include "GuiController.h"

namespace WL
{

	CGuiController::CGuiController()
	{

	}

	bool CGuiController::onEvent(event& e)
	{
		if ((e.message >= WM_MOUSEFIRST && e.message <= WM_MOUSELAST) ||
			(e.message >= WM_KEYFIRST && e.message <= WM_KEYLAST))
		{

		}
		return false;
	}

}