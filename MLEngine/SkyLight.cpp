#include "SkyLight.h"
#include "SkyLightComponent.h"
namespace ML
{
	CSkyLight::CSkyLight(const std::string& InName)
	: CLight(InName)
	{
		SkyLightComponent = SetDefaultSubobjectClass<CSkyLightComponent>();
	}

}
