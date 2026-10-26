#pragma once
#include "SceneComponent.h"
namespace ML
{
	class CWorld;
	class FPrimitiveSceneProxy;

	class MLENGINE_API CPrimitiveComponent : public CSceneComponent
    {
    public:
        CPrimitiveComponent();
        virtual ~CPrimitiveComponent();

        // UE: USceneComponent::SetVisibility
        // 游戏线程调用。改动会投递给渲染线程更新镜像（未注册时只改自己）。
        void SetVisible(bool bInVisible);
        bool IsVisible() const { return bVisible; }

        // UE: UPrimitiveComponent::CreateSceneProxy()。
        // 游戏线程调用：快照一份渲染线程用的可绘制数据，并把自己记到
        // RenderProxyId 上（之后改 Transform / 可见性才知道该更新谁）。
        // 基类没有可绘制网格，返回 nullptr；具体图元组件覆写它。
        virtual FPrimitiveSceneProxy* CreateSceneProxy();

        // UE: UPrimitiveComponent::RegisterComponentWithWorld(UWorld*)。
        // 在游戏线程创建场景代理，并把 AddPrimitive 投递给渲染线程。
        // CWorld 的注册流程只调这个入口，不认识具体组件类型。
        virtual void RegisterComponentWithWorld(CWorld* InWorld) override;

    protected:
    	bool bVisible = false;
    };
}
