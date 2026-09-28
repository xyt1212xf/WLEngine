#pragma once
#include "Common.h"
#include "Win.h"
#include "ThreadPool.h"
#include "Message.h"
#include "TSingle.h"
#include "Array.h"

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
	class CPlug;
	class CGraphicPlug;
	class CThreadPool;
	class CEngine : public TSingle<CEngine>
	{
	public:
		CEngine();
		virtual ~CEngine();
		bool Initialise(const SWindowConfig& config);
		bool UnInitialise();
		bool ProcessMsg(SEvent& e);
		void Run(int32 deltaSeconds);
		bool IsRun();
		CWinPlatform& GetPlatform();
			
		template<class F, class... Args>
		auto threadJoin(F&& f, Args&&... args);// ->std::future<typename std::result_of<F(Args...)>::type>;

		template<class F, class... Args>
		void threadDetach(F&& f, Args&&... args);// ->std::future<typename std::result_of<F(Args...)>::type>;		

		template<typename T>
		T* GetPlugs() const;

		template<typename T>
		void LoadResourceAsync(const std::string& resourceName) const;

		template<typename T>
		T* LoadResource(const std::string& resourceName) const;

	private:
		
		TArray<CPlug*> mPlugs;
		CWinPlatform mPlatform;
		CThreadPool* mpThreadPools = nullptr;
		bool mbRunning = false;
	};
	inline CEngine* GEngine = nullptr;


	template<typename T>
	T* ML::CEngine::GetPlugs() const
	{
		return nullptr;
	}

	template<class F, class... Args>
	auto CEngine::threadJoin(F&& f, Args&&... args)//->std::future<typename std::result_of<F(Args...)>::type>
	{
		return mpThreadPools->enqueueJoin(std::forward<F>(f), std::forward<Args>(args)...);
	}

	template<class F, class... Args>
	void CEngine::threadDetach(F&& f, Args&&... args)//->std::future<typename std::result_of<F(Args...)>::type>
	{
#ifdef _DEBUG
		if (nullptr != mpThreadPools)
		{
			mpThreadPools->enqueueDetach(std::forward<F>(f), std::forward<Args>(args)...);
		}
#else
		mpThreadPools->enqueueDetach(std::forward<F>(f), std::forward<Args>(args)...);
#endif
	}
}


#define Dev		ML::GEngine->getDevice()
#define GraphicDev	ML::GEngine->getGraphicsDevice();



