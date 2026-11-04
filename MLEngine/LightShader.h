#pragma once
#include "GlobalShader.h"
#include "Vector.h"

namespace ML
{
	struct FLightConstants
	{
		Vec4F LightDir;    // xyz = 指向光源的方向(归一化), w 留空
		Vec4F LightColor;  // rgb = 方向光颜色*强度,        a 留空
		Vec4F Ambient;     // rgb = 环境光,                 a 留空
		Vec4F Reserved;    // 对齐/扩展位
	};

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
