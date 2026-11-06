#include "PointLight.h"
#include "PointLightComponent.h"

namespace ML
{
	CPointLight::CPointLight(const std::string& InName)
	: CLight(InName)
	{
	 	PointLightComponent = SetDefaultSubobjectClass<CPointLightComponent>();
	}
}
