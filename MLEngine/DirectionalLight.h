#pragma once
#include "Light.h"
namespace ML
{
	class CDirectionalLightComponent;
	class CDirectionalLight : public CLight
	{
	public:
		explicit CDirectionalLight(const std::string& InName);
		CDirectionalLightComponent* GetDirectionalLightComponent() const { return DirectionalLightComponent; }

	private:
		CDirectionalLightComponent* DirectionalLightComponent = nullptr;
	};
}
