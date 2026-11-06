#include "DirectionalLight.h"
#include "DirectionalLightComponent.h"

namespace ML
{
	CDirectionalLight::CDirectionalLight(const std::string& InName)
	: CLight(InName)
	{
		DirectionalLightComponent = SetDefaultSubobjectClass<CDirectionalLightComponent>();
	}
}
