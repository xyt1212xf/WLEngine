#pragma once
#include "Common.h"
#include "RHIbuffer.h"
namespace ML
{
	struct FMeshVertexBuffers
	{
		CRHIBuffer* Position = nullptr;
		CRHIBuffer* Normal = nullptr;
		CRHIBuffer* Tangent = nullptr;
		CRHIBuffer* UV = nullptr;
		CRHIBuffer* Color = nullptr;

		void Release()
		{
			SafeDelete(Position);
			SafeDelete(Normal);
			SafeDelete(Tangent);
			SafeDelete(UV);
			SafeDelete(Color);
		}
		bool HasPosition() const { return nullptr != Position; }
		bool HasNormal()   const { return nullptr != Normal; }
		bool HasTangent()  const { return nullptr != Tangent; }
		bool HasUV()       const { return nullptr != UV; }
		bool HasColor()    const { return nullptr != Color; }
	};

	class CRHIBuffer;
	class MLENGINE_API CMeshLODResources
    {
    public:
        CMeshLODResources() = default;
        virtual ~CMeshLODResources();

		Vec3F GetBoundsOrigin() const { return BoundsOrigin;}
		float GetBoundsRadius() const { return BoundsRadius; }

	public:
		FMeshVertexBuffers VertexBuffers;   // 顶点缓冲聚合
		CRHIBuffer*        IndexBuffer = nullptr;  // 同级，不进聚合
		UINT32 NumVertices = 0;
		UINT32 NumIndices = 0;

	protected:
		// 包围球(UE 的 FBoxSphereBounds 简化为 原点 + 半径)
		Vec3F BoundsOrigin = Vec3F(0.0f, 0.0f, 0.0f);
		float BoundsRadius = 0.0f;
	};
}
