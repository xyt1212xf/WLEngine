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

struct VSIn
{
	float3 Pos   : POSITION;   // slot 0
	float4 Color : COLOR;      // slot 1
};

struct PSIn
{
	float4 Pos   : SV_POSITION;
	float4 Color : COLOR;
};

PSIn SolidColorVS(VSIn In)
{
	PSIn Out;
	// 行向量约定: 点 * MVP
	Out.Pos   = mul(float4(In.Pos, 1.0f), g_MVP);
	Out.Color = In.Color;
	return Out;
}

float4 SolidColorPS(PSIn In) : SV_Target
{
	return In.Color;
}
