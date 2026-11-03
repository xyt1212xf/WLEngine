#pragma once
#include "GlobalShader.h"

namespace ML
{
	class CLightVS : public CGlobalShader
	{
	public:
		DECLARE_GLOBAL_SHADER(CLightVS)
	};

	class CLightPS : public CGlobalShader
	{
	public:
		DECLARE_GLOBAL_SHADER(CLightPS)
	};
}
