#include "SkyLightComponent.h"
#include "Scene.h"
#include "SkyLightSceneProxy.h"

namespace ML
{

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

}
