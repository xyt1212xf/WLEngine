#pragma once
#include "Light.h"
namespace ML
{
	class CPointLightComponent;
	class CPointLight : public CLight
	{
	public:
		explicit CPointLight(const std::string& InName);
		CPointLightComponent* GetPointLightComponent() const { return PointLightComponent; };

	private:
		CPointLightComponent* PointLightComponent = nullptr;
	};
}
