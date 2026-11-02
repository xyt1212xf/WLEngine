//================================================================================
// Textured.hlsl — 带 base color 贴图的着色器 (UE: .usf 源文件)
//
// 顶点流布局对应 CStaticMeshLODResources 的 per-stream buffer:
//   slot 0: POSITION  float3   (PositionBuffer)
//   slot 1: COLOR     float4   (ColorBuffer)
//   slot 2: TEXCOORD0 float2   (UVBuffer)
//
// 根签名现在由 C++ 侧定义(见 DX12PipelineState::BuildRootSignature), 不再内嵌进
// 字节码。本文件只声明寄存器绑定: b0(MVP) / t0(贴图) / s0(采样器)。
// 与 SolidColor.hlsl 的差异只在 PS：把顶点色乘上贴图采样结果。
//================================================================================

// C++ 侧 FMatrix44 是行主序存储、行向量约定(点 * 矩阵), 所以这里 row_major。
row_major float4x4 g_MVP : register(b0);

// UE: UMaterialInterface 的 BaseColor 纹理参数 + 它的采样器状态。
Texture2D    g_BaseColor        : register(t0);
SamplerState g_BaseColorSampler : register(s0);

struct VSIn
{
	float3 Pos   : POSITION;   // slot 0
	float4 Color : COLOR;      // slot 1
	float2 UV    : TEXCOORD0;  // slot 2
};

struct PSIn
{
	float4 Pos   : SV_POSITION;
	float4 Color : COLOR;
	float2 UV    : TEXCOORD0;
};

PSIn TexturedVS(VSIn In)
{
	PSIn Out;
	// 行向量约定: 点 * MVP
	Out.Pos   = mul(float4(In.Pos, 1.0f), g_MVP);
	Out.Color = In.Color;
	Out.UV    = In.UV;
	return Out;
}

float4 TexturedPS(PSIn In) : SV_Target
{
	// 顶点色与贴图相乘：和 UE 里 BaseColor * VertexColor 的做法一致，
	// 这样贴上纯色贴图时原来那份棱角渐变色依然看得见。
	return In.Color * g_BaseColor.Sample(g_BaseColorSampler, In.UV);
}
