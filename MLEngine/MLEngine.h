#pragma once
#include "Win.h"
namespace ML
{
	enum eRT
	{
		DeviceRT = 0,
		LightRT = 1,
		FrontRT = 2,
	};
	class CVoxelMgr;
	class CScene;

	class CEngine : public CWinPlatform
	{
	public:
		CEngine();
		virtual ~CEngine();
	};
	inline CEngine* GEngine = nullptr;
}


#define Dev		ML::GEngine->getDevice()
#define GraphicDev	ML::GEngine->getGraphicsDevice();



