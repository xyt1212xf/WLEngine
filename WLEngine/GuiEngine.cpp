#include "GuiEngine.h"
#include "GuiEntity.h"
#include "GraphicEngine.h"
#include "WLEngine.h"
#include "UIFactory.h"

namespace WL
{
	std::list<CGuiEntity*>	CGuiEngine::mGuiEntites;
	std::list<CGuiEntity*>	CGuiEngine::mShowGuiEntites;
	std::map<CGuiEntity*, bool>	CGuiEngine::mCacheGuiEntities;
	CMutex CGuiEngine::gGuiMutex;
	CGuiEngine::CGuiEngine()
	{
		mpFactory = CUIFactory::createInstance();
	}

	CGuiEngine::~CGuiEngine()
	{
		mpFactory = nullptr;
		CUIFactory::destory();
	}

	CTexture* CGuiEngine::loadTexture(const std::string& szFile)
	{
		auto pTexture = dynamic_cast<CTexture*>(GEngine->createResource(szFile, Texture));
		if (nullptr != pTexture)
		{
			pTexture->addRef();
			auto pMaterial = dynamic_cast<CMaterial*>(GEngine->findResource(CGuiRendererPass::szMaterName));
			if (nullptr != pMaterial)
			{
				pTexture->setSampleState(pMaterial->getSampler());
			}
		}
		return pTexture;
	}

	bool CGuiEngine::initialize()
	{
		return true;
	}

	void CGuiEngine::unInitialize()
	{

	}

	void CGuiEngine::addGuiEntity(CGuiEntity* pGui)
	{
		auto iter = std::find(mGuiEntites.begin(), mGuiEntites.end(), pGui);
		if (iter == mGuiEntites.end())
		{
			mGuiEntites.push_back(pGui);
			if (pGui->isVisible())
			{
				CMutex::CAutoLock lock(CGuiEngine::gGuiMutex);
				mCacheGuiEntities[pGui] = true;
			}
		}
	}

	

	void CGuiEngine::removeGuiEntity(CGuiEntity* pGui)
	{
		{
			CMutex::CAutoLock lock(CGuiEngine::gGuiMutex);
			mCacheGuiEntities[pGui] = false;
		}
		auto item = std::find(mGuiEntites.begin(), mGuiEntites.end(), pGui);
		if (item != mGuiEntites.end())
		{
			mGuiEntites.erase(item);
		}
	}

	bool CGuiEngine::processMsg(SEvent& e)
	{
		switch (e.message)
		{
		case MSG_MOUSEMOVE:
		case MSG_LBUTTONDOWN:
		case MSG_LBUTTONUP:
		case MSG_RBUTTONDOWN:
		case MSG_RBUTTONUP:
		case MSG_MOUSEWHEEL:
		{
			CMutex::CAutoLock lock(CGuiEngine::gGuiMutex);
			for (auto item : mShowGuiEntites)
			{
				if (item->mLuaTableName.length() > 0)
				{
					int nPickId = item->mpUIWnd->_pick(e);
					if (-1 != nPickId)
					{
						lua_State* pLua = GEngine->getLuaState();
						int nTop = lua_gettop(pLua);
						lua_getglobal(pLua, item->mLuaTableName.c_str());
						if (lua_istable(pLua, -1))
						{
							lua_getfield(pLua, -1, "MsgProcess");
							if (lua_isfunction(GEngine->getLuaState(), -1))
							{
								lua_pushvalue(pLua, -2);
								lua_pushinteger(pLua, e.message);
								lua_pushinteger(pLua, nPickId);
								if (lua_pcall(pLua, 3, 1, 0) == LUA_OK)
								{
									int nRet = static_cast<int>(lua_tointeger(pLua, -1));
									e.bProcess = nRet == 0 ? false : true;
								}
							}
						}
						lua_settop(pLua, nTop);
						break;
					}
				}
			}
		}
		break;
		case MSG_KEYDOWN:
		case MSG_KEYUP:
		case MSG_CHAR:
		{
			lua_State* pLua = GEngine->getLuaState();
			CMutex::CAutoLock lock(CGuiEngine::gGuiMutex);
			for (auto item : mShowGuiEntites)
			{
				if (item->mLuaTableName.length() > 0)
				{
					int nTop = lua_gettop(pLua);
					lua_getglobal(pLua, item->mLuaTableName.c_str());
					if (lua_istable(pLua, -1))
					{
						lua_getfield(pLua, -1, "MsgProcess");
						if (lua_isfunction(GEngine->getLuaState(), -1))
						{
							lua_pushvalue(pLua, -2);
							lua_pushinteger(pLua, e.message);
							lua_pushinteger(pLua, (int)e.wParam);
							if (lua_pcall(pLua, 3, 1, 0) == LUA_OK)
							{
								int nRet = static_cast<int>(lua_tointeger(pLua, -1));
								e.bProcess = nRet == 0 ? false : true;
							}
						}
					}
					lua_settop(pLua, nTop);
				}
			}
			if (MSG_KEYUP == e.message)
			{
				auto iter = mShortCuts.find((CHAR)e.wParam);
				if (iter != mShortCuts.end())
				{

				}
			}
		}
		break;
		}
		if (!e.bProcess)
		{
			GEngine->reviceMsg(e);
		}
		return e.bProcess;
	}

	CUIWnd* CGuiEngine::createUI(WndType type)
	{
		return mpFactory->createUI(type);
	}

	void CGuiEngine::registerShortCuts(CHAR k, const std::string& szFunction)
	{
		mShortCuts[k].first = szFunction;
	}

	void CGuiEngine::registerShortCuts(CHAR k, std::function<bool(CHAR)> function)
	{
		mShortCuts[k].second = function;
	}

	void CGuiEngine::_update(UINT32 dTime)
	{
	}

}
