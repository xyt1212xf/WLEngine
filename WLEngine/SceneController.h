#pragma once
#include "Controller.h"
#include "Scene.h"

namespace WL
{
	class CSceneController : public CController
	{
	public:
		CSceneController(CScene* pScene);

	protected:
		virtual bool onEvent(event& e) final;

	protected:
		CScene* mpScene = nullptr;
	};
}