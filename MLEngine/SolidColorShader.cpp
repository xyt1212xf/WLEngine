#include "SolidColorShader.h"

namespace ML
{
	IMPLEMENT_GLOBAL_SHADER(CSolidColorVS, "SolidColor.hlsl", "SolidColorVS", "vs_5_0")
	IMPLEMENT_GLOBAL_SHADER(CSolidColorPS, "SolidColor.hlsl", "SolidColorPS", "ps_5_0")
}


