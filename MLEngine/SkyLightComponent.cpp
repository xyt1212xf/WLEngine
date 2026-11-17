#include "SkyLightComponent.h"
#include "Scene.h"
#include "SkyLightSceneProxy.h"
#include "World.h"
#include "MLEngine.h"

namespace ML
{


	CSkyLightComponent::~CSkyLightComponent()
	{
		if (SceneProxy)
		{
			if (GEngine)
			{
				if (CWorld* World = GEngine->GetWorld())
				{
					World->GetScene().DisableSkyLight(SceneProxy);
				}
			}
			SafeDelete(SceneProxy);
		}
	}

	CSkyLightSceneProxy* CSkyLightComponent::CreateSceneProxy()
	{
		CSkyLightSceneProxy::FInitParams Params;
		Params.Id = AllocateLightSceneProxyId();
		const Vec3F CI = GetColorIntensity();
		Params.LightColor = Vec4F(CI.x, CI.y, CI.z, 1.0f);
		Params.bVisible = IsVisible();
		// IrradianceSH / Cubemap：接上捕获/SH 计算流程后填充（UE 在 Initialize 时算）
		return new CSkyLightSceneProxy(Params);
	}


	void CSkyLightComponent::RegisterComponentWithWorld(CWorld* InWorld)
	{
		if (nullptr == InWorld)
		{
			return;
		}

		// UE: USkyLightComponent::CreateRenderState_Concurrent —— 组件自己 new
		// FSkyLightSceneProxy(this)，再 Scene->SetSkyLight(Proxy)。与 direct light
		// 不同：灯的 proxy 由 FScene::AddLight 内部工厂造，SkyLight 的由组件造。
		// SetSkyLight = 注册 + 激活一步完成（SkyLights 数组 + 当前生效指针）。
		if (nullptr == SceneProxy)
		{
			SceneProxy = CreateSceneProxy();
		}
		if (nullptr == SceneProxy)
		{
			return;
		}
		InWorld->GetScene().SetSkyLight(SceneProxy);
	}

}
