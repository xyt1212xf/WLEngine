//================================================================================
// SolidColor.hlsl — mini-UE 第一个全局 shader (UE: .usf 源文件)
//
// 顶点流布局对应 CStaticMeshLODResources 的 per-stream buffer:
//   slot 0: POSITION  float3   (PositionBuffer)
//   slot 1: COLOR     float4   (ColorBuffer)
//
// 根签名: 1 个根常量, 16 个 32bit 常量(4x4 MVP 矩阵)
//================================================================================

// 根签名内嵌进字节码, C++ 侧用 D3DGetBlobPart 提取后 CreateRootSignature,
// 保证根签名只有这一处定义
#define SOLID_COLOR_ROOT_SIG \
	"RootFlags( ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT ), " \
	"RootConstants( num32BitConstants=16, b0 )"

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

[RootSignature(SOLID_COLOR_ROOT_SIG)]
PSIn SolidColorVS(VSIn In)
{
	PSIn Out;
	// 行向量约定: 点 * MVP
	Out.Pos   = mul(float4(In.Pos, 1.0f), g_MVP);
	Out.Color = In.Color;
	return Out;
}

[RootSignature(SOLID_COLOR_ROOT_SIG)]
float4 SolidColorPS(PSIn In) : SV_Target
{
	return In.Color;
}
