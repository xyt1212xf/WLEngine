#include "InputMgr.h"
#include "GuiEngine.h"
#include "WLEngine.h"

namespace WL
{
	CInputMgr::CInputMgr()
	{

	}

	CInputMgr::~CInputMgr()
	{

	}

	bool CInputMgr::reviceMsg(event& e)
	{
		mMouseX = e.mouseX;
		mMouseY = e.mouseY;
		auto iter = mControllerMap.begin();
		auto end = mControllerMap.end();
		while (iter != end)
		{
			(iter->second)->onEvent(e);
			if (e.bProcess)
			{
				break;
			}
			++iter;
		}
		return e.bProcess;
	}

	bool CInputMgr::initialize()
	{
		return true;
	}

	void CInputMgr::unInitialize()
	{
		auto iter = mControllerMap.begin();
		auto iterEnd = mControllerMap.end();
		while (iter != iterEnd)
		{
			CController* p = (iter->second).get();
			WL_DELETE(p, Controller);
			(iter->second).release();
			++iter;
		}
		mControllerMap.clear();
	}

	void CInputMgr::registerController(CController* pController, INT32 nOrder)
	{
		if( -1 == nOrder)
		{
			int nSize = mControllerMap.size();
			mControllerMap[nOrder].reset(pController);
		}
		else
		{
			if (mControllerMap[nOrder].get() != pController)
			{
				mControllerMap[nOrder].release();
				mControllerMap[nOrder].reset(pController);
			}
		}
	}

	void CInputMgr::unRegisterController(CController* pController)
	{
		auto iter = mControllerMap.begin();
		auto iterEnd = mControllerMap.end();
		while( iter != iterEnd )
		{
			if ((iter->second).get() == pController)
			{
				mControllerMap.erase(iter);
				WL_DELETE(pController, Controller);
				break;
			}
		}
	}

	void CInputMgr::unRegisterController(INT32 nOrder)
	{
		auto iter = mControllerMap.find(nOrder);
		if( iter != mControllerMap.end() )
		{
			iter->second = nullptr;
			mControllerMap.erase(iter);
		}
	}



}