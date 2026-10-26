#include "PrimitiveComponent.h"
#include "Scene.h"
#include "World.h"
namespace ML
{
	CPrimitiveComponent::CPrimitiveComponent()
    {
    }
    CPrimitiveComponent::~CPrimitiveComponent()
    {
		if (0 != RenderProxyId)
		{
			const uint32 Id = RenderProxyId;
			RenderProxyId = 0;
			PushSceneCommand([Id](FScene& Scene) { Scene.RemovePrimitive(Id); });
		}
    }

	void CPrimitiveComponent::SetVisible(bool bInVisible)
	{
		if (bVisible == bInVisible)
		{
			return;
		}
		bVisible = bInVisible;

		if (0 == RenderProxyId)
		{
			return;
		}
		const uint32 Id = RenderProxyId;
		const bool bNowVisible = bVisible;
		PushSceneCommand([Id, bNowVisible](FScene& Scene)
			{
				Scene.SetVisibility(Id, bNowVisible);
			});
	}

	FPrimitiveSceneProxy* CPrimitiveComponent::CreateSceneProxy()
	{
		// UE: UPrimitiveComponent::CreateSceneProxy() —— 基类没有可绘制数据。
		// 具体图元组件（如 CStaticMeshComponent）覆写这里做快照。
		return nullptr;
	}

	void CPrimitiveComponent::RegisterComponentWithWorld(CWorld* InWorld)
	{
		if (nullptr == InWorld)
		{
			return;
		}

		// UE: UPrimitiveComponent 在注册时创建场景代理并加入 FScene。
		// proxy 在游戏线程构造（快照），AddPrimitive 投递给渲染线程执行；
		// CreateSceneProxy 已把 RenderProxyId 记好，命令 FIFO 保证
		// Add 一定排在它之后的 SetTransform / SetVisibility 之前。
		if (FPrimitiveSceneProxy* Proxy = CreateSceneProxy())
		{
			InWorld->EnqueueRenderCommand([Proxy](FScene& InScene)
				{
					InScene.AddPrimitive(Proxy);
				});
		}
	}
}
