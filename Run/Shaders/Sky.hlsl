//================================================================================
// Sky.hlsl — 程序化天空（UE: SkyAtmosphere 的最简替身）
//
// 一个全屏三角形铺满屏幕，不需要顶点缓冲，也不写深度：
//   * VS 只从 SV_VertexID 生成三个 NDC 角点；
//   * PS 用逆 ViewProj 把 NDC 反投影成世界空间视线方向，再按
//     "天顶 / 地平线 / 地面" 三段渐变 + 太阳盘与光晕着色。
//
// 绑定（根签名由 CRenderer::_InitSkyPass 按 FRootSignatureDesc 生成）：
//   b0 = InverseViewProj，顶点 + 像素都可见（bRootConstantsPixelVisible）
//   b2 = 灯光常量块，与 LightShader.h 的 FLightFrameConstants 完全一致；
//        太阳方向/颜色取自灯数组里的第一盏方向光，因此天空和场景光照一致。
//================================================================================

cbuffer SkyCB : register(b0)
{
    row_major float4x4 g_InvViewProj;
};

#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_POINT       1
#define LIGHT_TYPE_SPOT        2

// 与 C++ 侧 FSceneLightGPU (LightShader.h) 一致
struct SceneLight
{
    float4 ColorIntensity;  // rgb = 颜色*强度, a = 灯类型
    float4 Direction;       // xyz = 单位方向（方向光: 指向光源）
    float4 PositionRadius;  // xyz = 世界位置, w = 衰减半径
    float4 SpotAngles;      // x = cos(内锥半角), y = cos(外锥半角)
};

// 与 C++ 侧 FLightFrameConstants 一致
cbuffer Lighting : register(b2)
{
    float4     g_Ambient;
    float4     g_NumLights;
    SceneLight g_Lights[8];
};

// 天空配色。换成真实大气散射（Preetham / Hillaire）时只替换这一段的着色函数。
static const float3 ZenithColor  = float3(0.10f, 0.26f, 0.62f);
static const float3 HorizonColor = float3(0.66f, 0.76f, 0.86f);
static const float3 GroundColor  = float3(0.16f, 0.16f, 0.15f);

struct SkyVSOut
{
    float4 Pos : SV_POSITION;
    float2 Ndc : TEXCOORD0;
};

SkyVSOut SkyVS(uint VertexId : SV_VertexID)
{
    // 用一个大三角形盖住屏幕，而不是两个三角形拼 quad：
    //   v0 = (-1,-1)  v1 = (-1, 3)  v2 = (3,-1)
    // 超出屏幕的部分由光栅化裁掉，没有 overdraw 浪费。
    SkyVSOut Out;
    Out.Ndc = float2((2u == VertexId) ? 3.0f : -1.0f,
                     (1u == VertexId) ? 3.0f : -1.0f);
    Out.Pos = float4(Out.Ndc, 1.0f, 1.0f);
    return Out;
}

float4 SkyPS(SkyVSOut In) : SV_Target
{
    // 反投影 NDC 的近/远两点，相减得到世界空间视线方向。
    // 这样不需要相机位置常量，也不受投影矩阵具体形式影响。
    float4 NearH = mul(float4(In.Ndc, 0.0f, 1.0f), g_InvViewProj);
    float4 FarH  = mul(float4(In.Ndc, 1.0f, 1.0f), g_InvViewProj);
    float3 Dir   = normalize(FarH.xyz / FarH.w - NearH.xyz / NearH.w);

    const float Up = Dir.y;

    // 垂直渐变：地平线 -> 天顶。pow < 1 让过渡集中在地平线附近。
    const float3 Sky = lerp(HorizonColor, ZenithColor, pow(saturate(Up), 0.55f));

    // 地平线以下用地面雾色收住，相机俯视时不会看到突兀的纯色底。
    const float3 Ground = lerp(HorizonColor, GroundColor, saturate(-Up * 3.0f));
    float3 Color = (Up >= 0.0f) ? Sky : Ground;

    // 太阳 = 第一盏方向光。CPU 侧 Direction 是"指向光源"的方向。
    float3 SunDir   = float3(0.0f, 1.0f, 0.0f);
    float3 SunColor = float3(0.0f, 0.0f, 0.0f);
    const uint NumLights = (uint)g_NumLights.x;
    [loop]
    for (uint i = 0; i < 8 && i < NumLights; ++i)
    {
        if (LIGHT_TYPE_DIRECTIONAL == (uint)g_Lights[i].ColorIntensity.a)
        {
            SunDir   = normalize(g_Lights[i].Direction.xyz);
            SunColor = g_Lights[i].ColorIntensity.rgb;
            break;
        }
    }

    const float CosSun = saturate(dot(Dir, SunDir));

    // 太阳盘（硬边小圆）+ 两级光晕；天空是自发光，不受场景光照循环影响。
    const float Disc = smoothstep(0.9997f, 0.99995f, CosSun);
    const float Glow = pow(CosSun, 256.0f) * 0.6f + pow(CosSun, 16.0f) * 0.15f;
    Color += SunColor * (Disc * 6.0f + Glow);

    // 地平线暖雾（Mie 近似）：只在地平线一带、朝太阳那一侧出现。
    const float2 HorDir = normalize(Dir.xz + 1e-4f);
    const float2 SunHor = normalize(SunDir.xz + 1e-4f);
    const float  Haze   = pow(saturate(1.0f - abs(Up)), 8.0f) * saturate(dot(HorDir, SunHor));
    Color += SunColor * Haze * 0.12f;

    return float4(Color, 1.0f);
}
