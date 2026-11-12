#include "Light.h"
namespace ML
{

	CLight::CLight(const std::string& InName)
	: CActor(InName)
	{
		LightComponent = AddComponent<CLightComponent>();
		SetActorTickEnabled(false);   // UE: PrimaryActorTick.bCanEverTick = false
	}

}
