#pragma once
#include "GlobalShader.h"

namespace ML
{
	// UE: TShaderMap<FSolidColorVS> / a global-shader permutation.
	// Paired with CTexturedPS - both compiled from Shaders/Textured.hlsl.
	class CTexturedVS : public CGlobalShader
	{
	public:
		DECLARE_GLOBAL_SHADER(CTexturedVS)
	};

	class CTexturedPS : public CGlobalShader
	{
	public:
		DECLARE_GLOBAL_SHADER(CTexturedPS)
	};
}
