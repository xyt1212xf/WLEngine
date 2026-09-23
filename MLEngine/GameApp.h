#pragma once
#include "MLEngine.h"
#include "Message.h"
namespace ML
{
	class CGameApp
	{
	public:
		CGameApp();
		virtual ~CGameApp();
		bool entry(SWindowConfig& config);
		bool destoryEngine();
		void run();
	};

}