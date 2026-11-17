#include "DirectionalLightComponent.h"
#include "Scene.h"
#include "LightSceneProxy.h" 

namespace ML
{

	CLightSceneProxy* CDirectionalLightComponent::CreateSceneProxy()
	{
		CLightSceneProxy::FInitParams Params;
		Params.Id = AllocateLightSceneProxyId();
		Params.Type = ELightGPUType::Directional;
		const Vec3F CI = GetColorIntensity();
		Params.ColorIntensity = Vec4F(CI.x, CI.y, CI.z, 0.0f);
		Params.WorldMatrix = GetWorldMatrix();
		Params.bVisible = IsVisible();
		return new CLightSceneProxy(Params);   // ~CLightSceneProxy = delete：只能 new，由 CScene 持有
	}

}
