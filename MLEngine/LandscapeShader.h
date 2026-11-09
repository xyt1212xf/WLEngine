#pragma once
#include "GlobalShader.h"

namespace ML
{
	// UE: TShaderMap<FSolidColorVS> / a global-shader permutation.
	// Paired with CTexturedPS - both compiled from Shaders/Textured.hlsl.
	class CLandscapeVS : public CGlobalShader
	{
	public:
		DECLARE_GLOBAL_SHADER(CLandscapeVS)
	};

	class CLandscapePS : public CGlobalShader
	{
	public:
		DECLARE_GLOBAL_SHADER(CLandscapePS)
	};
}
