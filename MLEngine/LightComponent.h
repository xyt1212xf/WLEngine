#pragma once
#include "SceneComponent.h"
namespace ML
{
	enum class ELightMobility
	{
		Static,
		Stationary,
		Movable,
	};

	class MLENGINE_API CLightComponent : public CSceneComponent
	{
	public:
	    CLightComponent();
		virtual ~CLightComponent();

		bool IsVisible() const { return bVisible; }
		void SetVisibility(bool b) { bVisible = b; }  // 变化时 PushSceneCommand

		void SetLightColor(const Vec3F& InColor) { LightColor = InColor;}
		const Vec3F& GetLightColor() const { return LightColor; }

		void SetIntensity(float InIntensity) { Intensity = InIntensity; }
		float GetIntensity() const { return Intensity; }

		Vec3F GetColorIntensity() const { return LightColor * Intensity; } // GPU 侧合并

		ELightMobility GetMobility() const { return Mobility; }
		void SetMobility(ELightMobility InMobility) { Mobility = InMobility; }

	protected:
		bool bVisible = true;
		Vec3F LightColor = Vec3F(1, 1, 1);
		float Intensity = 1.0f;
		ELightMobility Mobility   = ELightMobility::Movable;
	};
}
