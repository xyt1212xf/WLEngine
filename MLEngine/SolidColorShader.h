#pragma once
#include "GlobalShader.h"

namespace ML
{
	class CSolidColorVS : public CGlobalShader
	{
	public:
		DECLARE_GLOBAL_SHADER(CSolidColorVS)
	};

	class CSolidColorPS : public CGlobalShader
	{
	public:
		DECLARE_GLOBAL_SHADER(CSolidColorPS)
	};
}
