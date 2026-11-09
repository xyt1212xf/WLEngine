#include "LandscapeShader.h"
namespace ML
{
	IMPLEMENT_GLOBAL_SHADER(CLandscapeVS, "Landscape.hlsl", "LandscapeVS", "vs_5_1")
	IMPLEMENT_GLOBAL_SHADER(CLandscapePS, "Landscape.hlsl", "LandscapePS", "ps_5_1")
}
