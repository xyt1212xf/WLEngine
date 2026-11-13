//================================================================================
// Sky.hlsl — 天空球材质（UE: SkyPass 的 unlit 天空材质）。
//
// 对标 UE5.6：天空 = 普通 mesh（SkySphere / HDRIBackdrop 的球体）+ "IsSky" 材质，
// 由 FSkyPassMeshProcessor 在 BasePass 之前、深度测试关闭下绘制。
// 视线方向取球面顶点法线（球心即相机：法线 == 该像素视线方向）。
//
// 绑定与 Lighting.hlsl 一致（复用同一套根签名）：
//   b0 = MVP（顶点 root constants），b1 = World，b2 = 灯光常量块。
//   太阳取灯数组里第一盏方向光（UE: SkyAtmosphere 的太阳即场景方向光）。
//================================================================================

cbuffer TransformCB : register(b0)
{
    row_major float4x4 g_MVP;
};

cbuffer WorldCB : register(b1)
{
    row_major float4x4 g_World;
};

#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_POINT       1
#define LIGHT_TYPE_SPOT        2

struct SceneLight
{
    float4 ColorIntensity;  // rgb = 颜色*强度, a = 灯类型
    float4 Direction;       // xyz = 单位方向（方向光: 指向光源）
    float4 PositionRadius;  // xyz = 世界位置, w = 衰减半径
    float4 SpotAngles;      // x = cos(内锥半角), y = cos(外锥半角)
};

cbuffer Lighting : register(b2)
{
    float4     g_Ambient;
    float4     g_NumLights;
    SceneLight g_Lights[8];
}

static const float3 ZenithColor  = float3(0.10f, 0.26f, 0.62f);
static const float3 HorizonColor = float3(0.66f, 0.76f, 0.86f);
static const float3 GroundColor  = float3(0.16f, 0.16f, 0.15f);

struct VSIn
{
    float3 Pos    : POSITION;    // slot 0
    float3 Normal : NORMAL;      // slot 1
    float2 UV     : TEXCOORD0;   // slot 2
    float4 Color  : COLOR;       // slot 3
};

struct PSIn
{
    float4 Pos    : SV_POSITION;
    float3 Normal : NORMAL;      // 世界空间法线 = 视线方向
    float2 UV     : TEXCOORD0;
};

PSIn SkyVS(VSIn In)
{
    PSIn Out;
    Out.Pos    = mul(float4(In.Pos, 1.0f), g_MVP);
    Out.Normal = mul((float3x3)g_World, In.Normal);
    Out.UV     = In.UV;
    return Out;
}

// unlit：天空自发光，不吃灯光循环（UE: unlit shading model）
float4 SkyPS(PSIn In) : SV_Target
{
    const float3 Dir = normalize(In.Normal);
    const float  Up  = Dir.y;

    const float3 Sky    = lerp(HorizonColor, ZenithColor, pow(saturate(Up), 0.55f));
    const float3 Ground = lerp(HorizonColor, GroundColor, saturate(-Up * 3.0f));
    float3 Color = (Up >= 0.0f) ? Sky : Ground;

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
    const float Disc   = smoothstep(0.9997f, 0.99995f, CosSun);
    const float Glow   = pow(CosSun, 256.0f) * 0.6f + pow(CosSun, 16.0f) * 0.15f;
    Color += SunColor * (Disc * 6.0f + Glow);

    const float2 HorDir = normalize(Dir.xz + 1e-4f);
    const float2 SunHor = normalize(SunDir.xz + 1e-4f);
    const float  Haze   = pow(saturate(1.0f - abs(Up)), 8.0f) * saturate(dot(HorDir, SunHor));
    Color += SunColor * Haze * 0.12f;

    return float4(Color, 1.0f);
}
