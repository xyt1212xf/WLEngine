#pragma once
#include "Vector.h"
#include "RHIbuffer.h"
#include "MeshAttributes.h"
#include "Array.h"

namespace ML
{
	// FStaticMeshLODResources：一个 LOD 烘焙后的渲染数据。
	// UE5 对标：FStaticMeshLODResources（StaticMeshResources.h）。
	//
	// 持有：
	//   - per-stream 顶点缓冲（与 CStaticVertexFactory 的 SDataStreamUnit 对应）
	//   - 索引缓冲
	//   - Sections（按材质分段，定义在 MeshAttributes.h）
	//   - 包围球（剔除用）
	//
	// 注意：FStaticMeshSection 定义在 MeshAttributes.h（烘焙输出端），
	// 这里直接复用，避免两处定义漂移。
	class CStaticVertexFactory;

	class CStaticMeshLODResources
	{
	public:
		CStaticMeshLODResources() = default;
		~CStaticMeshLODResources();

		// 从烘焙结果(CMeshAttributes 平铺数据）创建渲染资源。
		// 当前阶段：拷贝 CPU 侧元数据(Sections/数量/包围球)，
		// VB/IB 的 GPU 上传留到 P1 接通 CDX12RHIbuffer 创建后实现。
		void InitResources(const CMeshAttributes& Attrs);

		// 释放全部资源。
		void ReleaseResources();

		// 访问器
		const TArray<FStaticMeshSection>& GetSections() const { return Sections; }
		UINT32 GetNumVertices() const { return NumVertices; }
		UINT32 GetNumIndices()  const { return NumIndices; }

		const Vec3F& GetBoundsOrigin() const { return BoundsOrigin; }
		float        GetBoundsRadius() const { return BoundsRadius; }

		// 顶点数据流(per-stream，与 VertexStreamFactory 对齐)
		CRHIBuffer* PositionBuffer = nullptr;
		CRHIBuffer* TangentBuffer  = nullptr;   // tangent + UV 打包流（P1）
		CRHIBuffer* ColorBuffer    = nullptr;
		CRHIBuffer* IndexBuffer    = nullptr;

		TArray<FStaticMeshSection> Sections;

		// 包围球(UE 的 FBoxSphereBounds 简化为 原点 + 半径)
		Vec3F BoundsOrigin = Vec3F(0.0f, 0.0f, 0.0f);
		float BoundsRadius = 0.0f;

		CStaticVertexFactory* VertexFactory = nullptr;

		UINT32 NumVertices = 0;
		UINT32 NumIndices  = 0;
	};
}