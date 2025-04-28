#pragma once
#include "WLEngine.h"

namespace WL
{
	class CGameApp
	{
	public:
		CGameApp();
		virtual ~CGameApp();
		bool entry(WindowConfig& config);
		bool destoryEngine();
		void run();

	private:
		CEngine* mpEngine = nullptr;
	};

}