#pragma once
#include "MeshComponent.h"
#include "Matrix.h"
#include "Quaternion.h"
#include "Vector.h"
namespace ML
{
	class CStaticMesh;
	class FPrimitiveSceneProxy;

	class MLENGINE_API CStaticMeshComponent : public CMeshComponent
	{
	public:
		CStaticMeshComponent();
		virtual ~CStaticMeshComponent();

		// UE: UStaticMeshComponent::CreateSceneProxy() override
		// 游戏线程调用：快照一份渲染线程用的可绘制数据，并把自己记到
		// RenderProxyId 上（之后改 Transform / 可见性才知道该更新谁）。
		// 返回 nullptr 表示网格还没烘好，注册不了。
		virtual FPrimitiveSceneProxy* CreateSceneProxy() override;

		CStaticMesh* GetStaticMesh() const { return StaticMesh; }
		void SetStaticMesh(CStaticMesh* InMesh) { StaticMesh = InMesh; }
		
	protected:
		CStaticMesh* StaticMesh = nullptr;
	};
}
