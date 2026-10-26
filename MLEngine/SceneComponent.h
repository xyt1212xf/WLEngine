#pragma once
#include "ActorComponent.h"
#include "Vector.h"
#include "Quaternion.h"
#include "Matrix.h"

namespace ML
{
	class FScene;

	class MLENGINE_API CSceneComponent : public CActorComponent
    {
    public:
        CSceneComponent();
        virtual ~CSceneComponent();
		void SetLocation(const Vec3F& InLocation);
		void SetRotation(const CQuaternion& InRotation);
		void SetScale(const Vec3F& InScale);
		const Vec3F& GetLocation() const { return Location; }
		const CQuaternion& GetRotation() const { return Rotation; }
		const Vec3F& GetScale() const { return Scale; }

		// UE: USceneComponent::SetupAttachment / K2_AttachToComponent.
		// Location/Rotation/Scale here are RELATIVE to AttachParent
		// (UE: RelativeLocation / RelativeRotation / RelativeScale3D);
		// GetWorldMatrix() composes them with every ancestor up to the root.
		void AttachToComponent(CSceneComponent* InParent) { AttachParent = InParent; }
		CSceneComponent* GetAttachParent() const { return AttachParent; }

		// UE: USceneComponent::GetComponentTransform()
		FMatrix44 GetWorldMatrix() const;

		// ---- 渲染线程镜像 ----
		// UE: FPrimitiveSceneProxy 的 id。0 = 还没注册进 FScene
		// （此时改动只会写组件自身，不会投递渲染命令）。
		uint32 GetRenderProxyId() const { return RenderProxyId; }
		void   SetRenderProxyId(uint32 InId) { RenderProxyId = InId; }

	protected:
		// 游戏线程：向渲染线程投递一条场景更新命令。
		// RenderProxyId == 0（未注册）或引擎还没起时自动丢弃。
		void PushSceneCommand(std::function<void(FScene&)> InCommand);

		// 游戏线程：Transform 变了，通知渲染线程更新镜像。
		// 世界矩阵在**游戏线程这边算好**再按值投递，渲染线程拿到的
		// 是一个快照，不会读到写了一半的矩阵。
		void MarkRenderTransformDirty();

		uint32      RenderProxyId = 0;

	protected:
		Vec3F       Location;
		CQuaternion Rotation = CQuaternion(0.0f, 0.0f, 0.0f, 1.0f);
		Vec3F       Scale = Vec3F(1, 1, 1);
		CSceneComponent* AttachParent = nullptr;

    };
}
