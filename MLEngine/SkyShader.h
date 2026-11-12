#pragma once
#include "GlobalShader.h"

namespace ML
{
	// UE: TShaderMap<FSolidColorVS> / a global-shader permutation.
	// Paired with CTexturedPS - both compiled from Shaders/Textured.hlsl.
	class CSkyVS : public CGlobalShader
	{
	public:
		DECLARE_GLOBAL_SHADER(CSkyVS)
	};

	class CSkyPS : public CGlobalShader
	{
	public:
		DECLARE_GLOBAL_SHADER(CSkyPS)
	};
}
