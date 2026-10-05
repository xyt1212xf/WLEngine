#pragma once
#include "Refcount.h"
namespace ML
{
	enum class EStreamingState : uint8
	{
		Unloaded,
		Loading,
		Loaded,
		Unloading
	};


    class CLevel;
	
	using FOnLevelLoaded = std::function<void(CLevel*)>;
	
	class CLevelStreaming : public CRefcount
    {
    public:	
        CLevelStreaming();
        virtual ~CLevelStreaming();
		void Update(float DeltaTime);
		void SetOnLoadedCallback(FOnLevelLoaded Callback) { OnLoaded = Callback; }
		void RequestLoad();
		void RequestUnload();
		
		EStreamingState GetState() const { return State; }
		CLevel* GetLevel() const { return Level; }
		std::string GetLevelName() const { return LevelName; }
		void SetLoadDistance(float Distance ) { mLoadDistance = Distance;}
		float GetLoadDistance() const {return mLoadDistance;}
		bool IsLoaded() 
		{
			return EStreamingState::Loaded == State;
		}

    private:
		EStreamingState State = EStreamingState::Unloaded;
		std::string LevelName;     // 关联的关卡名
		std::string FilePath;      // 关卡文件路径
		CLevel* Level = nullptr;   // 加载完成后指向实际的 Level
		float mLoadDistance = 1000.f;

		FOnLevelLoaded OnLoaded;
		// 加载请求
		bool bShouldBeLoaded = false;
		bool bShouldBeVisible = false;
    };

	
}