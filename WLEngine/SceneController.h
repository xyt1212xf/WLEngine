#pragma once
#include "Controller.h"
#include "Scene.h"

namespace WL
{
	class CSceneController : public SController
	{
	public:
		CSceneController(CScene* pScene);

	protected:
		virtual bool onEvent(SEvent& e) final;

	protected:
		CScene* mpScene = nullptr;
	};
}