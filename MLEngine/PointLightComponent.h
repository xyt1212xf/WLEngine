#pragma once
#include "LightComponent.h"
namespace ML
{
	class MLENGINE_API CPointLightComponent : public CLightComponent
	{
	public:
		void  SetAttenuationRadius(float InRadius) { AttenuationRadius = InRadius; }
		float GetAttenuationRadius() const { return AttenuationRadius; }

		// UE: UPointLightComponent::LightFalloffExponent，默认 2.0
		void  SetFalloffExponent(float InExponent) { FalloffExponent = InExponent; }
		float GetFalloffExponent() const { return FalloffExponent; }

	protected:
		float AttenuationRadius = 10.0f;
		float FalloffExponent = 2.0f;
	};
}

