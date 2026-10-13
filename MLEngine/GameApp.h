#pragma once
#include "MLEngine.h"
#include "Message.h"
namespace ML
{
	class MLENGINE_API CGameApp
	{
	public:
		CGameApp();
		virtual ~CGameApp();
		bool entry(SWindowConfig& config);
		bool destoryEngine();
		void run();
		void loadScene(const std::string& name);
	};

}
