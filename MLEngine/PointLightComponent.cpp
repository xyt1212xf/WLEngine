#include "PointLightComponent.h"
#include "Scene.h"
#include "LightSceneProxy.h" 

namespace ML
{

	CLightSceneProxy* CPointLightComponent::CreateSceneProxy()
	{
		CLightSceneProxy::FInitParams Params;
		Params.Id = AllocateLightSceneProxyId();
		Params.Type = ELightGPUType::Point;
		const Vec3F CI = GetColorIntensity();
		Params.ColorIntensity = Vec4F(CI.x, CI.y, CI.z, 0.0f);
		Params.WorldMatrix = GetWorldMatrix();
		Params.Radius = AttenuationRadius;         // UE: AttenuationRadius
		Params.FalloffExponent = FalloffExponent;
		Params.bVisible = IsVisible();
		return new CLightSceneProxy(Params);
	}

}
