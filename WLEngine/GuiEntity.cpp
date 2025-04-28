#include "GuiEntity.h"
#include "WLEngine.h"
#include "UIWnd.h"
#include "Scene.h"

namespace WL
{
	DefineScriptClass(CGuiEntity)
	
	struct CWndCallBack : public CCallBack
	{
		CWndCallBack()
		{

		}
		CWndCallBack(void* pParameters)
		: CCallBack(pParameters)
		{
		}
		virtual ~CWndCallBack()
		{
			mpWnd = nullptr;
		}
		virtual void onRun(void* pParameters)
		{
		}
		CUIWnd* mpWnd = nullptr;
	};

	void CGuiEntity::registerClass()
	{
		CRegisterClass<CGuiEntity>::registerClass(GEngine->getLuaState());
	}

	void CGuiEntity::registerMember()
	{
		auto pLua = GEngine->getLuaState();
		CRegisterFun<bool>::registerClassMemberFun<CGuiEntity, bool(CGuiEntity::*)(const std::string&), &CGuiEntity::initialise, const std::string&>(pLua, "initialise");
		CRegisterFun<void>::registerClassMemberFun<CGuiEntity, void(CGuiEntity::*)(const std::string&), &CGuiEntity::setLuaEntityName, const std::string&>(pLua, "setLuaEntityName");
		CRegisterFun<void>::registerClassMemberFun<CGuiEntity, void(CGuiEntity::*)(bool), &CGuiEntity::setVisible, bool>(pLua, "setVisible");
		CRegisterFun<bool>::registerClassMemberFun<CGuiEntity, bool(CGuiEntity::*)()const, &CGuiEntity::isVisible>(pLua, "isVisible");
		CRegisterFun<float>::registerClassMemberFun<CGuiEntity, float(CGuiEntity::*)()const, &CGuiEntity::getX>(pLua, "getX");
		CRegisterFun<float>::registerClassMemberFun<CGuiEntity, float(CGuiEntity::*)()const, &CGuiEntity::getY>(pLua, "getY");
		CRegisterFun<void>::registerClassMemberFun<CGuiEntity, void(CGuiEntity::*)(float), &CGuiEntity::setX,float>(pLua, "setX");
		CRegisterFun<void>::registerClassMemberFun<CGuiEntity, void(CGuiEntity::*)(float), &CGuiEntity::setY,float>(pLua, "setY");
		CRegisterFun<void>::registerClassMemberFun<CGuiEntity, void(CGuiEntity::*)(float,float), &CGuiEntity::setPositon,float,float>(pLua, "setPositon");

		CRegisterFun<void>::registerClassMemberFun<CGuiEntity, void(CGuiEntity::*)(const std::string&), &CGuiEntity::addChildGui, const std::string&>(pLua, "addChildGui");
		CRegisterFun<void>::registerClassMemberFun<CGuiEntity, void(CGuiEntity::*)(const std::string&), &CGuiEntity::loadGui, const std::string&>(pLua, "loadGui");
		CRegisterFun<void>::registerClassMemberFun<CGuiEntity, void(CGuiEntity::*)(const std::string&), &CGuiEntity::setAnimation, const std::string&>(pLua, "setAnimation");
		CRegisterFun<void>::registerClassMemberFun<CGuiEntity, void(CScriptEntity::*)(), &CScriptEntity::enterScene>(pLua, "enterScene");
		CRegisterFun<void>::registerClassMemberFun<CGuiEntity, void(CScriptEntity::*)(), &CScriptEntity::leaveScene>(pLua, "leaveScene");

		CRegisterFun<void>::registerClassMemberFun<CGuiEntity, void(CGuiEntity::*)(const std::string&), &CGuiEntity::setText, const std::string&>(pLua, "setText");
		CRegisterFun<void>::registerClassMemberFun<CGuiEntity, void(CGuiEntity::*)(UINT32, const std::string&), &CGuiEntity::setText, UINT32, const std::string&>(pLua, "setChildText");
		CRegisterFun<void>::registerClassMemberFun<CGuiEntity, void(CGuiEntity::*)(bool), &CGuiEntity::setAniLoop, bool>(pLua, "setAniLoop");
		CRegisterFun<bool>::registerClassMemberFun<CGuiEntity, bool(CGuiEntity::*)()const, &CGuiEntity::isAniLoop>(pLua, "isAniLoop");
		CRegisterFun<bool>::registerClassMemberFun<CGuiEntity, bool(CGuiEntity::*)(int), &CGuiEntity::setCurrentFrame, int>(pLua, "setCurrentFrame");
		CRegisterFun<int>::registerClassMemberFun<CGuiEntity, int(CGuiEntity::*)()const, &CGuiEntity::getCurrentFrame>(pLua, "getCurrentFrame");
		CRegisterFun<void>::registerClassMemberFun<CGuiEntity, void(CGuiEntity::*)(float), &CGuiEntity::setAniSpeed, float>(pLua, "setAniSpeed");
		CRegisterFun<float>::registerClassMemberFun<CGuiEntity, float(CGuiEntity::*)()const, &CGuiEntity::getAniSpeed>(pLua, "getAniSpeed");

		CRegisterFun<bool>::registerClassMemberFun<CGuiEntity, bool(CGuiEntity::*)(), &CGuiEntity::isXYAxisSwap>(pLua, "isXYAxisSwap");
		CRegisterFun<bool>::registerClassMemberFun<CGuiEntity, bool(CGuiEntity::*)(UINT32), &CGuiEntity::isXYAxisSwap, UINT32>(pLua, "isChildXYAxisSwap");
		CRegisterFun<void>::registerClassMemberFun<CGuiEntity, void(CGuiEntity::*)(bool), &CGuiEntity::setXYAxisSwap, bool>(pLua, "setXYAxisSwap");
		CRegisterFun<void>::registerClassMemberFun<CGuiEntity, void(CGuiEntity::*)(UINT32, bool), &CGuiEntity::setXYAxisSwap, UINT32, bool>(pLua, "setChildXYAxisSwap");
	}

	CGuiEntity::CGuiEntity()
	{
		mType = Gui;
		setVisible(false);
	}

	CGuiEntity::~CGuiEntity()
	{
		if (GEngine)
		{
			GEngine->removeGuiEntity(this);
			WL_DECREASE(mpUIWnd);
		}
	}

	void CGuiEntity::_tick(UINT32 dt)
	{
		for(auto item : mpUIWnd->mChildren)
		{
			item->_tick(dt);
		}
	}

	bool CGuiEntity::initialise(const std::string& szFile)
	{
		bool bResult = false;
		auto pWnd = dynamic_cast<CUIWnd*>(GEngine->createResource(szFile, ResourceType::Wnd));
		if( nullptr != pWnd )
		{
			WL_INCREASE(pWnd);
			WL_DECREASE(mpUIWnd)
			mpUIWnd = pWnd;

			CWndCallBack* pCallBack = new CWndCallBack(nullptr);
			pCallBack->mpWnd = this->mpUIWnd;
			pCallBack->mTimerID = 0;
			pCallBack->mAlternationTime = 5000;
			pCallBack->mRunCount = -1;
			GEngine->registerCall(pCallBack, -1);
			GEngine->addGuiEntity(this);
			bResult = true;
		}
		return bResult;
	}

	void CGuiEntity::addChildGui(const std::string& szFile)
	{
		if (nullptr != mpUIWnd)
		{
			auto pWnd = dynamic_cast<CUIWnd*>(GEngine->createResource(szFile, ResourceType::Wnd));
			if (nullptr != pWnd)
			{
				pWnd->setVisible(true);
				mpUIWnd->addChild(pWnd);
				WL_DECREASE(pWnd);
			}
		}
	}

	void CGuiEntity::loadGui(const std::string& szFile)
	{
		CResource* pResource =  GEngine->createResource(ResourceType::Wnd);
		bool bResult = GEngine->loadResource(pResource, szFile);
		if (bResult)
		{
			auto pWnd = dynamic_cast<CUIWnd*>(pResource);
			mpUIWnd = pWnd;

			CWndCallBack* pCallBack = new CWndCallBack(nullptr);
			pCallBack->mpWnd = this->mpUIWnd;
			pCallBack->mTimerID = 0;
			pCallBack->mAlternationTime = 5000;
			pCallBack->mRunCount = -1;
			GEngine->registerCall(pCallBack, -1);
			GEngine->addGuiEntity(this);
		}
	}

	void CGuiEntity::setAnimation(const std::string& szFile)
	{
		if (nullptr != mpUIWnd)
		{
			for (auto item : mpUIWnd->mChildren)
			{
				if (ui_animation == item->getWndType())
				{
					item->setTexture(szFile);
					break;
				}
			}
		}
	}

	void CGuiEntity::setLuaEntityName(const std::string& szName)
	{
		mLuaTableName = szName;
	}

	void CGuiEntity::setPositon(float x, float y)
	{
		setX(x);
		setY(y);
	}

	void CGuiEntity::setX(float x)
	{
		if( nullptr != mpUIWnd )
		{
			auto v = mpUIWnd->getPosition();
			v.x = x;
			mpUIWnd->setPosition(v);
		}
	}

	void CGuiEntity::setY(float y)
	{
		if (nullptr != mpUIWnd)
		{
			auto v = mpUIWnd->getPosition();
			v.y = y;
			mpUIWnd->setPosition(v);
		}
	}

	float CGuiEntity::getX() const
	{
		if (nullptr != mpUIWnd)
		{
			auto v = mpUIWnd->getPosition();
			return v.x;
		}
	}

	float CGuiEntity::getY() const
	{
		if (nullptr != mpUIWnd)
		{
			auto v = mpUIWnd->getPosition();
			return v.y;
		}
	}

	void CGuiEntity::setWidth(float w)
	{
		if (nullptr != mpUIWnd)
		{
			auto size = mpUIWnd->getSize();
			size.x = w;
			mpUIWnd->setSize(size);
		}
	}

	void CGuiEntity::setHeight(float h)
	{
		if (nullptr != mpUIWnd)
		{
			auto size = mpUIWnd->getSize();
			size.x = h;
			mpUIWnd->setSize(size);
		}
	}

	void CGuiEntity::setVisible(bool bValue)
	{
		if (isVisible() != bValue)
		{
			CEntity::setVisible(bValue);
			GEngine->removeGuiEntity(this);
			GEngine->addGuiEntity(this);
		}
	}

	void CGuiEntity::setVisible(UINT32 Id, bool bValue)
	{
		if (nullptr != mpUIWnd)
		{
			auto pChild = mpUIWnd->getchild(Id);
			if (pChild.has_value())
			{
				pChild.value()->setVisible(bValue);
			}
		}
	}

	void CGuiEntity::setPosition(const Vec3F& pos)
	{
		if (nullptr != mpUIWnd)
		{
			mpUIWnd->setPosition(pos);
		}
	}

	void CGuiEntity::setPosition(UINT32 Id, const Vec3F& pos)
	{
		if( nullptr != mpUIWnd )
		{
			auto pChild = mpUIWnd->getchild(Id);
			if( pChild.has_value() )
			{
				pChild.value()->setPosition(pos);
			}
		}
	}

	void CGuiEntity::setSize(UINT32 Id, const Vec2F& size)
	{
		if (nullptr != mpUIWnd)
		{
			auto pChild = mpUIWnd->getchild(Id);
			if (pChild.has_value())
			{
				pChild.value()->setSize(size);
			}
		}
	}

	void CGuiEntity::setSize(const Vec2F& szie)
	{
		if (nullptr != mpUIWnd)
		{
			mpUIWnd->setSize(szie);
		}
	}

	void CGuiEntity::setRotation(UINT32 Id, float fRadion)
	{
		if (nullptr != mpUIWnd)
		{
			auto pChild = mpUIWnd->getchild(Id);
			if (pChild.has_value())
			{
				pChild.value()->setRotation(fRadion);
			}
		}
	}

	void CGuiEntity::setRotation(float fRadion)
	{
		if( nullptr != mpUIWnd )
		{
			mpUIWnd->setRotation(fRadion);
		}
	}

	float CGuiEntity::getRotation() const
	{
		return mpUIWnd ? mpUIWnd->getRotation() : 0.0f;
	}

	float CGuiEntity::getRotation(UINT32 Id) const
	{
		if (nullptr != mpUIWnd)
		{
			auto pChild = mpUIWnd->getchild(Id);
			if (pChild.has_value())
			{
				return pChild.value()->getRotation();
			}
		}
		return 0.0f;
	}


	void CGuiEntity::setText(const std::string& szContent)
	{
		if (nullptr != mpUIWnd)
		{
			mpUIWnd->setText(szContent);
		}
	}

	void CGuiEntity::setText(UINT32 Id, const std::string& szContent)
	{
		if( nullptr != mpUIWnd )
		{
			auto pChild = mpUIWnd->getchild(Id);
			if(pChild.has_value())
			{
				pChild.value()->setText(szContent);
			}
		}
	}

	bool CGuiEntity::isVisible() const
	{
		return CEntity::isVisible();
	}

	bool CGuiEntity::isVisible(UINT32 id) const 
	{
		auto pWnd = mpUIWnd->getchild(id);
		return pWnd.has_value() ? pWnd.value()->isVisible() : false;
	}


	void CGuiEntity::setAniLoop(bool bLoop)
	{
		mbLoop = bLoop;
		if (nullptr != mpUIWnd)
		{
			for (auto item : mpUIWnd->mChildren)
			{
				if (ui_animation == item->getWndType())
				{
					item->setAniLoop(bLoop);
					break;
				}
			}
		}
	}
	
	bool CGuiEntity::isAniLoop()const
	{
		return mbLoop;
	}

	bool CGuiEntity::setCurrentFrame(int nNumber)
	{
		if (nullptr != mpUIWnd)
		{
			for (auto item : mpUIWnd->mChildren)
			{
				if (ui_animation == item->getWndType())
				{
					return item->setCurrentFrame(nNumber);
				}
			}
		}
		return false;
	}
	
	int CGuiEntity::getCurrentFrame() const
	{
		if (nullptr != mpUIWnd)
		{
			for (auto item : mpUIWnd->mChildren)
			{
				if (ui_animation == item->getWndType())
				{
					return item->getCurrentFrame();
				}
			}
		}
	}

	void CGuiEntity::setAniSpeed(float fSpeed)
	{
		if (nullptr != mpUIWnd)
		{
			for (auto item : mpUIWnd->mChildren)
			{
				if (ui_animation == item->getWndType())
				{
					item->setAniSpeed(fSpeed);
					break;
				}
			}
		}
	}

	float CGuiEntity::getAniSpeed() const
	{
		if (nullptr != mpUIWnd)
		{
			for (auto item : mpUIWnd->mChildren)
			{
				if (ui_animation == item->getWndType())
				{
					return item->getAniSpeed();
				}
			}
		}
		return 0;
	}

	bool CGuiEntity::isXYAxisSwap(UINT32 Id)
	{
		if (nullptr != mpUIWnd)
		{
			auto pChild = mpUIWnd->getchild(Id);
			if (pChild.has_value())
			{
				return pChild.value()->isXYAxisSwap();
			}
			else
			{
				return false;
			}
		}
		return false;
	}

	void CGuiEntity::setXYAxisSwap(bool bValue)
	{
		if (nullptr != mpUIWnd)
		{
			mpUIWnd->setXYAxisSwap(bValue);
		}
	}

	bool CGuiEntity::isXYAxisSwap()
	{
		return nullptr != mpUIWnd ? mpUIWnd->isXYAxisSwap() : false;
	}

	void CGuiEntity::setXYAxisSwap(UINT32 Id, bool bValue)
	{
		if (nullptr != mpUIWnd)
		{
			auto pChild = mpUIWnd->getchild(Id);
			if (pChild.has_value())
			{
				pChild.value()->setXYAxisSwap(bValue);
			}
		}
	}

}