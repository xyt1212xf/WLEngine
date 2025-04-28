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
		bool reviceMsg(event& e);
		bool initialize();
		void unInitialize();
		void registerController(CController* pController, INT32 nOrder);
		void unRegisterController(CController* pController);
		void unRegisterController(INT32 nOrder);

	private:
		int mMouseX;
		int mMouseY;
		std::map<INT32, std::unique_ptr<CController>>	mControllerMap;
	};
}