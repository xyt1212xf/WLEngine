#include "GameController.h"
namespace WL
{
	CGameController::CGameController()
	{

	}

	bool CGameController::onEvent(event& e)
	{
		switch (e.message)
		{
		case MSG_RBUTTONDOWN:
		{
			break;
		}
		case MSG_RBUTTONUP:
		{
			break;
		}
		case MSG_MOUSEMOVE:
		{
			break;
		}
		case MSG_KEYDOWN:
		{
			break;
		}
		case MSG_KEYUP:
		{
			break;
		}
		}
		return false;
	}
}