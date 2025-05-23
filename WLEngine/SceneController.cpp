#include "SceneController.h"
#include "WLEngine.h"

namespace WL
{
	CSceneController::CSceneController(CScene* pScene)
	{
		mpScene = pScene;
	}

	bool CSceneController::onEvent(SEvent& e)
	{
		return false;
	}
}

