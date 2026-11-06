#pragma once
#include "Light.h"

namespace ML
{
	class CSkyLightComponent;
	class CSkyLight : public CLight
	{
	public:
		explicit CSkyLight(const std::string& InName);
		CSkyLightComponent* GetSkyLightComponent() const { return SkyLightComponent; }

	private:
		CSkyLightComponent* SkyLightComponent = nullptr;
	};
}
