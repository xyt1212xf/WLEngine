#pragma once
#include "Info.h"
#include "SkyLightComponent.h"

namespace ML
{
	class MLENGINE_API CSkyLight : public CInfo
	{
	public:
		explicit CSkyLight(const std::string& InName);
		CSkyLightComponent* GetLightComponent() const { return LightComponent; }

		void SetLightColor(const Vec3F& InColor) { LightComponent->SetLightColor(InColor); }
		void SetLightBrightness(float InBrightness) { LightComponent->SetIntensity(InBrightness); }

	protected:
		template<class T>
		T* SetLightComponent()
		{
			static_assert(std::is_convertible_v<T*, CSkyLightComponent*>,
				"SetLightComponent<T>: T must derive publicly from CSkyLightComponent");

			if (LightComponent)
			{
				RemoveComponent(LightComponent);
			}
			LightComponent = AddComponent<T>();
			SetRootComponent(LightComponent);       // UE: RootComponent = LightComponent;
			return static_cast<T*>(LightComponent);
		}

	private:
		CSkyLightComponent* LightComponent = nullptr;
	};
}
