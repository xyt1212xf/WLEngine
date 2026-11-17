#pragma once
#include "LightDefinitions.h"

namespace ML
{
	/** Different light component types. The enum uses values defined in a shared header with shader code so that the two sides are always consistent. */
	enum ELightComponentType
	{
		LightType_Directional = LIGHT_TYPE_DIRECTIONAL,
		LightType_Point = LIGHT_TYPE_POINT,
		LightType_Spot = LIGHT_TYPE_SPOT,
		LightType_Rect = LIGHT_TYPE_RECT,
		LightType_MAX = LIGHT_TYPE_MAX,
		LightType_NumBits = 2
	};

}
