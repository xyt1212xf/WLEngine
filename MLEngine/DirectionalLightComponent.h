#pragma once
#include "LightComponent.h"
namespace ML
{
	// UE: UDirectionalLightComponent : ULightComponent
	// 没有位置和范围，只有朝向（取 Actor 的 Forward）；无限远平行光。
	class MLENGINE_API CDirectionalLightComponent : public CLightComponent
	{
	public:
		// UE: UDirectionalLightComponent::LightSourceAngle（太阳张角，度），
		// 影响软阴影；当前没有 shadow map，先把字段留着。
		void  SetLightSourceAngle(float InAngle) { LightSourceAngle = InAngle; }
		float GetLightSourceAngle() const { return LightSourceAngle; }

	protected:
		float LightSourceAngle = 0.5357f;
	};
}
