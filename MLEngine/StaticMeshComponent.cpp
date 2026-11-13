#include "StaticMeshComponent.h"
#include "StaticMeshResources.h"
#include "Scene.h"
#include "StaticMesh.h"
#include "VertexStreamFactory.h"

namespace ML
{

	CStaticMeshComponent::CStaticMeshComponent()
	{

	}

	CStaticMeshComponent::~CStaticMeshComponent()
	{

	}

	CPrimitiveSceneProxy* CStaticMeshComponent::CreateSceneProxy()
	{
		// 网格没烘好（LOD / VertexFactory 为空）时不注册；proxy 内部也会
		// 再判一次，这里提前挡掉只是为了不白分配一个 id。
		if (nullptr == StaticMesh
			|| nullptr == StaticMesh->GetRenderData(0)
			|| nullptr == StaticMesh->GetVertexFactory(0))
		{
			return nullptr;
		}

		// id 必须在游戏线程这边先拿到：渲染命令是 FIFO 的，保证随后投递的
		// SetTransform / SetVisibility 一定排在 AddPrimitive 之后。
		RenderProxyId = AllocateSceneProxyId();
		CMaterial* Mat = GetOverrideMaterial(0);
		if (nullptr == Mat)
		{
			Mat = StaticMesh->GetMaterialForSection(0);
		}
		return new CPrimitiveSceneProxy(RenderProxyId, StaticMesh,
			StaticMesh->GetRenderData(0), StaticMesh->GetVertexFactory(0),
			Mat, GetWorldMatrix(), 0, IsVisible());
	}
}
