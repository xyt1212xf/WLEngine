#pragma once
#include "Common.h"
#include "Win.h"
#include "Message.h"
#include "TSingle.h"


namespace ML
{
	enum eRT
	{
		DeviceRT = 0,
		LightRT = 1,
		FrontRT = 2,
	};
	//class CVoxelMgr;
	//class CScene;
	class CGraphicPlug;
	class CEngine : public TSingle<CEngine>
	{
	public:
		CEngine();
		virtual ~CEngine();
		bool Initialise(const SWindowConfig& config);
		bool UnInitialise();
		bool processMsg(SEvent& e);
		CWinPlatform& GetPlatform();

		template<typename T>
		T* GetPlugs() const;

	private:
		//TArray<CPlug*> mPlugs;
		CGraphicPlug* mGraphicPlug = nullptr;
		CWinPlatform mPlatform;
	};
	inline CEngine* GEngine = nullptr;


	template<typename T>
	T* ML::CEngine::GetPlugs() const
	{
		return nullptr;
	}


}


#define Dev		ML::GEngine->getDevice()
#define GraphicDev	ML::GEngine->getGraphicsDevice();



