//================================================================================
// SolidColor.hlsl — mini-UE 第一个全局 shader (UE: .usf 源文件)
//
// 顶点流布局对应 CStaticMeshLODResources 的 per-stream buffer:
//   slot 0: POSITION  float3   (PositionBuffer)
//   slot 1: COLOR     float4   (ColorBuffer)
//
// 根签名: 1 个根常量, 16 个 32bit 常量(4x4 MVP 矩阵)。
// 现在由 C++ 侧定义(见 DX12PipelineState::BuildRootSignature), 不再内嵌进字节码;
// 本文件只声明寄存器绑定 b0(MVP)。
//================================================================================

// C++ 侧 FMatrix44 是行主序存储、行向量约定(点 * 矩阵),
// 因此这里声明 row_major, 上传前不需要转置
// 注意: MVP 的组合顺序应为 World * View * Proj (行向量约定从左到右依次变换)
row_major float4x4 g_MVP : register(b0);
row_major float4x4 g_World : register(b1);
cbuffer Lighting : register(b2)
{
    float4 g_LightDir;    // xyz = 方向(指向光源, 已归一化), w 留空
    float4 g_LightColor;  // rgb = 颜色*强度
    float4 g_Ambient;     // rgb = 环境光
    float4 g_Reserved;    // 对齐用, 以后可放强度/衰减
};

Texture2D    g_BaseTexture        : register(t0);
SamplerState g_BaseTextureSampler : register(s0);

struct VSIn
{
	float3 Pos		: POSITION;   // slot 0
	float3 Normal 	: NORMAL;		//slot 1
	float2 UV 		: TEXCOORD0;	//slot 2
	float4 Color 	: COLOR;      // slot 3
};

struct PSIn
{
	float4 Pos   	: SV_POSITION;
	float3 Normal 	: NORMAL;	
	float2 UV 		: TEXCOORD0;	
	float4 Color 	: COLOR;
};

PSIn LightVS(VSIn In)
{
	PSIn Out;
	// 行向量约定: 点 * MVP
	Out.Pos   = mul(float4(In.Pos, 1.0f), g_MVP);
	Out.UV    = In.UV;
	Out.Normal = mul((float3x3)g_World, In.Normal);
	Out.Color = In.Color;
	return Out;
}

float4 LightPS(PSIn In) : SV_Target
{
	float3 N = normalize(In.Normal);
    float3 L = normalize(g_LightDir.xyz);
    float NdotL = max(dot(N, L), 0.0f);
    float3 ambient = g_Ambient.rgb;
    float3 diffuse = g_LightColor.rgb * NdotL;
    float4 tex = g_BaseTexture.Sample(g_BaseTextureSampler, In.UV);
    float3 color = tex.rgb * (ambient + diffuse) * In.Color.rgb;
    return float4(color, tex.a * In.Color.a);
}
