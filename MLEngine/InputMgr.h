#pragma once
#include <map>
#include "Message.h"
#include "Controller.h"
#include "Vector.h"
#include "CameraEntity.h"

namespace WL
{
	class CInputMgr 
	{
	public:
		CInputMgr();
		virtual ~CInputMgr();
		bool reviceMsg(SEvent& e);
		bool initialize();
		void unInitialize();
		void registerController(SController* pController, INT32 nOrder);
		void unRegisterController(SController* pController);
		void unRegisterController(INT32 nOrder);

	private:
		int mMouseX;
		int mMouseY;
		std::map<INT32, std::unique_ptr<SController>>	mControllerMap;
	};
}