#pragma once
#include <shared_mutex>
#include <functional>
#include "InputMgr.h"
#include "EngineBase.h"
#include "UIFactory.h"
#include "Mutex.h"

namespace WL
{
	class CGuiEntity;
	class CUIFactory;
	class CGuiEngine : virtual public CEngineBase 
	{
		friend class CGuiRendererPass;

	public:
		CGuiEngine();
		virtual ~CGuiEngine();
		static CTexture* loadTexture(const std::string& szFile);

		bool initialize();
		void unInitialize();
		void addGuiEntity(CGuiEntity* pGui);
		void removeGuiEntity(CGuiEntity* pGui);
		bool processMsg(event& e);
		CUIWnd* createUI(WndType type);
		void registerShortCuts(CHAR k, const std::string& szFunction);
		void registerShortCuts(CHAR k, std::function<bool(CHAR)> function);

	protected:
		virtual void _update(UINT32 dTime);
		static std::list<CGuiEntity*>	mGuiEntites;
		static std::list<CGuiEntity*>	mShowGuiEntites;
		static std::map<CGuiEntity*, bool>	mCacheGuiEntities;
		static CMutex  gGuiMutex;
	private:
		CUIFactory* mpFactory = nullptr;
		std::map<CHAR, std::pair<std::string, std::function<bool(CHAR)> > > mShortCuts;
	};
}

