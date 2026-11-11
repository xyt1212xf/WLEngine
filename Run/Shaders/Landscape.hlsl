//================================================================================
// Landscape.hlsl — 地形着色器：base color 分层混合 + 场景光照
//   对齐 Lighting.hlsl 的光照通路；根签名每个材质 PSO 都已启用
//   b0=MVP / b1=World / b2=光照CBV / t0=分层贴图(Texture2DArray) / s0=采样器
//================================================================================

//row_major float4x4 g_MVP   : register(b0);
//row_major float4x4 g_World : register(b1);

cbuffer TransformCB : register(b0)
{
    row_major float4x4 g_MVP;
};

cbuffer WorldCB : register(b1)
{
    row_major float4x4 g_World;
};

// 灯类型, 与 C++ 侧 ELightGPUType (LightShader.h) 一致
#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_POINT       1
#define LIGHT_TYPE_SPOT        2

// 单灯快照: 4 x float4 = 64 字节, 与 C++ 侧 FSceneLightGPU 一致
struct SceneLight
{
    float4 ColorIntensity;  // rgb = 颜色*强度(CPU 侧已合并), a = 灯类型
    float4 Direction;       // xyz = 单位方向。方向光: 指向光源; 聚光: 照射方向
    float4 PositionRadius;  // xyz = 世界位置(点光/聚光), w = 衰减半径(0 = 无限)
    float4 SpotAngles;      // x = cos(内锥半角), y = cos(外锥半角)
};

// per-frame 灯光常量块, 与 C++ 侧 FLightFrameConstants 一致
cbuffer Lighting : register(b2)
{
    float4     g_Ambient;    // rgb = 环境光
    float4     g_NumLights;  // x = 本帧灯数
    SceneLight g_Lights[8];
};

// Landscape layer blend (UE: LandscapeLayerBlend / weightmap):
//   t0 is ONE Texture2DArray packed from up to 4 albedo layers, in the order
//   declared by the material's Layers table. Per-vertex blend weights ride in
//   the COLOR vertex stream (r = slice 0 base, g = slice 1 rock, b = slice 2
//   snow, a = slice 3 spare) - built by CLandscapeLODResources from height /
//   slope, moving to painted weightmaps later. The renderer binds this array
//   exactly like a single base-color texture, so the root signature and the
//   draw path are unchanged.
Texture2DArray g_LayerTextures    : register(t0);
SamplerState   g_BaseColorSampler : register(s0);

// Detail tiling: the geometry UV spans 0..1 across one tile, so the layer
// textures are repeated this many times per tile (wrap sampler).
#define LAYER_UV_SCALE 16.0f

struct VSIn
{
    float3 Pos    : POSITION;   // slot 0
    float3 Normal : NORMAL;     // slot 1
    float2 UV     : TEXCOORD0;  // slot 2
    float4 Color  : COLOR;      // slot 3
};

struct PSIn
{
    float4 Pos      : SV_POSITION;
    float3 WorldPos : WORLDPOS;  // 点光/聚光衰减需要世界位置
    float3 Normal   : NORMAL;
    float2 UV       : TEXCOORD0;
    float4 Color    : COLOR;
};

PSIn LandscapeVS(VSIn In)
{
    PSIn Out;
    Out.Pos      = mul(float4(In.Pos, 1.0f), g_MVP);
    Out.WorldPos = mul(float4(In.Pos, 1.0f), g_World).xyz;
    Out.Normal   = mul((float3x3)g_World, In.Normal);
    Out.UV       = In.UV;
    Out.Color    = In.Color;
    return Out;
}

// UE: InvSquared 距离衰减(Falloff Exponent = 2):
//   window = saturate(1 - (d/r)^4), atten = window^2 / (d^2 + 1)
float ComputeDistanceAttenuation(SceneLight L, float DistanceToLight)
{
    float d2 = DistanceToLight * DistanceToLight;
    float Radius = L.PositionRadius.w;
    if (Radius <= 0.0f)
        return 1.0f / (d2 + 1.0f);
    float Window = saturate(1.0f - pow(DistanceToLight / Radius, 4.0f));
    return Window * Window / (d2 + 1.0f);
}

float3 ShadeOneLight(SceneLight L, float3 N, float3 WorldPos)
{
    float3 Ldir;
    float  atten = 1.0f;
    uint   Type  = (uint)L.ColorIntensity.a;

    if (Type == LIGHT_TYPE_DIRECTIONAL)
    {
        Ldir = normalize(L.Direction.xyz);
    }
    else
    {
        float3 ToLight = L.PositionRadius.xyz - WorldPos;
        float  dist    = length(ToLight);
        float  Radius  = L.PositionRadius.w;
        if (Radius > 0.0f && dist > Radius)
            return float3(0.0f, 0.0f, 0.0f);
        Ldir  = ToLight / max(dist, 1e-4f);
        atten = ComputeDistanceAttenuation(L, dist);

        if (Type == LIGHT_TYPE_SPOT)
        {
            float cosA = dot(-Ldir, normalize(L.Direction.xyz));
            atten *= smoothstep(L.SpotAngles.y, L.SpotAngles.x, cosA);
        }
    }

    float NdotL = saturate(dot(N, Ldir));
    return L.ColorIntensity.rgb * (NdotL * atten);
}

float4 LandscapePS(PSIn In) : SV_Target
{
    float3 N = normalize(In.Normal);

    float3 lighting = g_Ambient.rgb;
    uint NumLights = (uint)g_NumLights.x;
    [loop]
    for (uint i = 0; i < 8 && i < NumLights; ++i)
    {
        lighting += ShadeOneLight(g_Lights[i], N, In.WorldPos);
    }

    // Layer blend: weights from the COLOR stream, re-normalized so a vertex
    // whose weights do not sum to 1 (e.g. precision at tile borders) still
    // produces a fully covered surface.
    float4 w = saturate(In.Color);
    float wsum = w.r + w.g + w.b + w.a;
    w = (wsum > 1e-5f) ? (w / wsum) : float4(1.0f, 0.0f, 0.0f, 0.0f);

    float2 uv = In.UV * LAYER_UV_SCALE;
    float3 albedo =
        w.r * g_LayerTextures.Sample(g_BaseColorSampler, float3(uv, 0.0f)).rgb +
        w.g * g_LayerTextures.Sample(g_BaseColorSampler, float3(uv, 1.0f)).rgb +
        w.b * g_LayerTextures.Sample(g_BaseColorSampler, float3(uv, 2.0f)).rgb +
        w.a * g_LayerTextures.Sample(g_BaseColorSampler, float3(uv, 3.0f)).rgb;

    return float4(albedo * lighting, 1.0f);
}
