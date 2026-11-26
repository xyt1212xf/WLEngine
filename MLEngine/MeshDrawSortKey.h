#pragma once
#include "Common.h"
#include "MeshBatch.h"
#include "ViewMatrices.h"

namespace ML
{
	
	// UE5.6 对标: Engine/Source/Runtime/Renderer/Public/MeshPassProcessor.h 的 FMeshDrawCommandSortKey。
	// 把排序维度打包进一个 uint64（PackedData），operator< 一次整数比较完成多维排序；
	// 值小的先画（UE 注释: smaller values are drawn first）。
	//
	// 字段名与分组顺序对齐 UE5.6；位宽按本引擎数据规模取整（UE 位宽以引擎源码为准）：
	//   - VertexShaderHash / PixelShaderHash 在 UE 里是 shader 哈希（PSO 的核心）；
	//     本引擎 PSO = (Material, VertexFactory, Pass)（见 CMaterial::GetPipelineState），
	//     所以 PixelShaderHash 槽位填 Material 指针哈希（材质决定 VS/PS/纹理），
	//     VertexShaderHash 槽位填 VertexFactory 指针哈希（决定输入布局）。
	//     两槽合起来等价于 UE 的 "PSOId"，同值 ⇒ 同 PSO ⇒ 聚组免切换。
	//   - MeshIdInPrimitive = SectionIndex，确定性 tie-break（同 PSO 内稳定次序）。
	//   - 哈希冲突无害：只影响聚组质量，不影响正确性（同 UE 的哈希排序语义）。
	struct FMeshDrawCommandSortKey
	{
		union
		{
			// Generic 视图（UE5.6: { Priority, VertexShaderHash, PixelShaderHash, MeshIdInPrimitive }）
			struct
			{
				uint64 MeshIdInPrimitive : 16;	// 低位：先比较（tie-break）
				uint64 PixelShaderHash : 20;	// 本引擎 = Material 指针哈希
				uint64 VertexShaderHash : 20;	// 本引擎 = VertexFactory 指针哈希
				uint64 Priority : 8;	// 高位：优先级（当前恒 0）
			} Generic;

			// BasePass 视图（UE5.6: { Background, BasePass, Masked, shader hashes, MeshIdInPrimitive }）
			// Masked/背景位在高处 ⇒ 特殊 BasePass 绘制先聚组（本引擎暂无 Masked，恒 0，留作挂钩）。
			struct
			{
				uint64 MeshIdInPrimitive : 16;
				uint64 PixelShaderHash : 20;
				uint64 VertexShaderHash : 20;
				uint64 Masked : 2;	// UE: bIsMasked
				uint64 BasePass : 2;	// UE: BasePass priority bits
				uint64 Background : 2;	// UE: background 绘制优先
			} BasePass;

			// Translucent 视图（UE5.6: { PixelShaderHash, Distance, MeshIdInPrimitive }）
			// Distance 存"反转后的视轴距离量化值"：值小先画 ⇒ 远的先画（back-to-front）。
			struct
			{
				uint64 MeshIdInPrimitive : 16;
				uint64 Distance : 28;	// 反转量化视轴距离（步进 4.0f）
				uint64 PixelShaderHash : 20;	// 混合模式分组（同 UE）
			} Translucent;

			uint64 PackedData = 0;
		};

		bool operator<(const FMeshDrawCommandSortKey& B) const { return PackedData < B.PackedData; }
		bool operator!=(const FMeshDrawCommandSortKey& B) const { return PackedData != B.PackedData; }
	};
	// UE: PointerHash —— 指针混洗到 20 位哈希。指针低位对齐无信息量，先移位再混合。
	FORCEINLINE uint32 MeshDrawHashPointer(const void* P)
	{
		uint64 x = reinterpret_cast<uint64>(P);
		x ^= (x >> 33);
		x *= 0xFF51AFD7ED558CCDull;
		x ^= (x >> 29);
		return (uint32)((x >> 32) & 0xFFFFFull);
	}

	// UE5.6: CalculateMeshStaticSortKey —— 通用 pass（SkyPass 等特殊 pass 用）。
	FORCEINLINE FMeshDrawCommandSortKey CalculateMeshStaticSortKey(const FMeshBatch& Batch)
	{
		FMeshDrawCommandSortKey Key;
		Key.Generic.MeshIdInPrimitive = Batch.SectionIndex & 0xFFFFull;
		Key.Generic.PixelShaderHash = Batch.Material ? MeshDrawHashPointer(Batch.Material) : 0ull;
		Key.Generic.VertexShaderHash = Batch.VertexFactory ? MeshDrawHashPointer(Batch.VertexFactory) : 0ull;
		Key.Generic.Priority = 0ull;
		return Key;
	}
	FORCEINLINE FMeshDrawCommandSortKey CalculateBasePassMeshStaticSortKey(const FMeshBatch& Batch, bool bIsMasked = false)
	{
		FMeshDrawCommandSortKey Key;
		Key.BasePass.MeshIdInPrimitive = Batch.SectionIndex & 0xFFFFull;
		Key.BasePass.PixelShaderHash = Batch.Material ? MeshDrawHashPointer(Batch.Material) : 0ull;
		Key.BasePass.VertexShaderHash = Batch.VertexFactory ? MeshDrawHashPointer(Batch.VertexFactory) : 0ull;
		Key.BasePass.Masked = bIsMasked ? 1ull : 0ull;
		Key.BasePass.BasePass = 0ull;
		Key.BasePass.Background = 0ull;
		return Key;
	}
	// UE5.6: CalculateTranslucentMeshStaticSortKey —— 半透明静态部分。
		// Distance 沿视轴（UE 默认 TranslucentSortPolicy = SortByDistanceAlongViewAxis，
		// 而非欧氏距离），量化步进 4.0f 后反转，保证升序排序时"远 → 近"。
	FORCEINLINE FMeshDrawCommandSortKey CalculateTranslucentMeshStaticSortKey(
		const FMeshBatch& Batch, const FViewMatrices& ViewMatrices)
	{
		constexpr float QuantStep = 4.0f;
		constexpr uint64 MaxQuant = (1ull << 28) - 1ull;

		const Vec3F WorldPos(Batch.WorldMatrix.m[3][0], Batch.WorldMatrix.m[3][1], Batch.WorldMatrix.m[3][2]);
		// 行向量约定：ViewMatrix 第 2 行 = 相机 forward（z 轴）
		const Vec3F Forward(ViewMatrices.ViewMatrix.m[2][0], ViewMatrices.ViewMatrix.m[2][1], ViewMatrices.ViewMatrix.m[2][2]);
		const Vec3F Rel = WorldPos - ViewMatrices.ViewOrigin;
		const float ViewAxisDist = Rel.x * Forward.x + Rel.y * Forward.y + Rel.z * Forward.z;

		uint64 Quant = 0ull;
		if (ViewAxisDist > 0.0f)
		{
			Quant = (uint64)(ViewAxisDist / QuantStep);
			if (Quant > MaxQuant)
			{
				Quant = MaxQuant;
			}
		}

		FMeshDrawCommandSortKey Key;
		Key.Translucent.MeshIdInPrimitive = Batch.SectionIndex & 0xFFFFull;
		Key.Translucent.Distance = MaxQuant - Quant;   // 反转：远 ⇒ 值小 ⇒ 先画
		Key.Translucent.PixelShaderHash = Batch.Material ? MeshDrawHashPointer(Batch.Material) : 0ull;
		return Key;
	}

}
