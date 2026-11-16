#pragma once
#include "SceneComponent.h"
namespace ML
{
	class MLENGINE_API CLightComponentBase : public CSceneComponent
	{
	public:
		CLightComponentBase();
		bool IsVisible() const { return bVisible; }


		void SetLightColor(const Vec3F& InColor) { LightColor = InColor; }
		const Vec3F& GetLightColor() const { return LightColor; }

		void SetIntensity(float InIntensity) { Intensity = InIntensity; }
		float GetIntensity() const { return Intensity; }

		Vec3F GetColorIntensity() const { return LightColor * Intensity; }   // GPU 侧合并

		bool AffectsWorld() const { return bAffectsWorld; }

	protected:
		bool   bAffectsWorld = true;          // UE: ULightComponentBase::bAffectsWorld
		uint32 CastShadows : 1;               // UE: ULightComponentBase::CastShadows，构造里初始化
		float  IndirectLightingIntensity = 1.0f;   // UE: ULightComponentBase::IndirectLightingIntensity
		float  Intensity = 1.0f;              // UE: ULightComponentBase::Intensity
		Vec3F  LightColor = Vec3F(1, 1, 1);   // UE: ULightComponentBase::LightColor
	};
}
