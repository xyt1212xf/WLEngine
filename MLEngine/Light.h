#pragma once
#include "Actor.h"
#include "LightComponent.h"

namespace ML
{
    class MLENGINE_API CLight : public CActor
    {
    public:
		explicit CLight(const std::string& InName);

		// UE: ALight::GetLightComponent()
		CLightComponent* GetLightComponent() const { return LightComponent; }

		void SetLightColor(const Vec3F& InColor) { LightComponent->SetLightColor(InColor); }
		void SetLightBrightness(float InBrightness) { LightComponent->SetIntensity(InBrightness); }
		float GetLightBrightness() const { return LightComponent->GetIntensity(); }


	protected:
		template<class T>
		T* SetDefaultSubobjectClass()
		{
			static_assert(std::is_convertible_v<T*, CLightComponent*>,
				"SetDefaultSubobjectClass<T>: T must derive publicly from CLightComponent");

			RemoveComponent(LightComponent);          // UE: 替换默认子对象
			LightComponent = AddComponent<T>();
			SetRootComponent(LightComponent);         // UE: RootComponent = LightComponent;
			return static_cast<T*>(LightComponent);
		}

	protected:
		CLightComponent* LightComponent = nullptr;
    };
}
