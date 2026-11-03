#include "LightShader.h"

namespace ML
{
	IMPLEMENT_GLOBAL_SHADER(CLightVS, "Lighting.hlsl", "LightVS", "vs_5_1")
	IMPLEMENT_GLOBAL_SHADER(CLightPS, "Lighting.hlsl", "LightPS", "ps_5_1")		
}


