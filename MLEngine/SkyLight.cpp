#include "SkyLight.h"
#include "SkyLightComponent.h"
namespace ML
{
	CSkyLight::CSkyLight(const std::string& InName)
	: CInfo(InName)
	{
		LightComponent = SetLightComponent<CSkyLightComponent>();
	}

}
