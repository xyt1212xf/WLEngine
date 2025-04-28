#pragma once
#include "EngineBase.h"
namespace WL
{
	class CSoundEngine : virtual public CEngineBase
	{
	public:
		CSoundEngine();
		virtual ~CSoundEngine();
		bool initialize();
		void unInitialize();
	protected:
		virtual void _update(UINT32 dTime);
	};
}

