#pragma once
#include "GlobalShader.h"
#include "Vector.h"

namespace ML
{
	constexpr uint32 MaxSceneLights = 8;

	enum class ELightGPUType : uint32
	{
		Directional = 0,   // 方向光：只用 Direction，无位置/衰减
		Point = 1,   // 点光：PositionRadius + 距离衰减
		Spot = 2,   // 聚光：点光 + SpotAngles 锥角衰减
	};


	struct FLightConstants
	{
		Vec4F LightDir;    // xyz = 指向光源的方向(归一化), w 留空
		Vec4F LightColor;  // rgb = 方向光颜色*强度,        a 留空
		Vec4F Ambient;     // rgb = 环境光,                 a 留空
		Vec4F Reserved;    // 对齐/扩展位
	};

	// 单灯快照：4 x float4 = 64 字节
	struct FSceneLightGPU
	{
		Vec4F ColorIntensity;  // rgb = 颜色*强度（CPU 侧已合并），a = ELightGPUType
		Vec4F Direction;       // xyz = 单位方向。方向光：指向光源；聚光：照射方向
		Vec4F PositionRadius;  // xyz = 世界位置（点光/聚光），w = 衰减半径（0 = 无限）
		Vec4F SpotAngles;      // x = cos(内锥半角)，y = cos(外锥半角)，z/w 留空
	};

	// per-frame 灯光常量块（引擎一帧一份，root CBV 绑定 b2）
	struct FLightFrameConstants
	{
		Vec4F Ambient;         // rgb = 环境光
		Vec4F NumLightsPad;    // x = 本帧灯数，y/z/w 留空
		FSceneLightGPU Lights[MaxSceneLights];
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
