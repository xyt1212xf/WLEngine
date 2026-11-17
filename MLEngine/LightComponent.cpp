#include "LightComponent.h"
#include "LightSceneProxy.h" 
#include "MLEngine.h"
#include "World.h"

namespace ML
{

	CLightComponent::CLightComponent()
	{

	}

	CLightComponent::~CLightComponent()
	{
		if (SceneProxy)
		{
			if (GEngine)
			{
				if (CWorld* World = GEngine->GetWorld())
				{
					World->GetScene().RemoveLight(SceneProxy);
				}
			}
			SceneProxy = nullptr;
		}
	}


	void CLightComponent::RegisterComponentWithWorld(CWorld* InWorld)
	{
		if (nullptr == InWorld)
		{
			return;
		}

		// UE: ULightComponent::OnRegister → FScene::AddLight(this)（游戏线程直调）。
		// 入口收组件而不是 proxy：此刻 proxy 还不存在，由场景内部调用
		// 本组件的 CreateSceneProxy() 工厂创建，场景里只存 proxy。
		InWorld->GetScene().AddLight(this);
	}

}
