#pragma once
#include "SceneComponent.h"

namespace ML
{
	struct FMinimalViewInfo
	{
		Vec3F Location;            // UE: FMinimalViewInfo::Location
		Vec3F Rotation;            // UE: Rotation（本引擎直接存 Forward，见 GetCameraView）
		float FOV = 90.0f;         // UE: FOV（水平，度）
		float NearZ = 10.0f;       // UE: NearClippingPlane
		float FarZ = 5000.0f;      // UE: FarClippingPlane
		float AspectRatio = 0.0f;  // UE: AspectRatio（0 = 跟随视口宽高比）
	};
	class MLENGINE_API CCameraComponent : public CSceneComponent
	{
	public:
		CCameraComponent() = default;
		virtual ~CCameraComponent() = default;
		void SetFieldOfView(float InFOV) { FieldOfView = InFOV; }
		float GetFieldOfView() const { return FieldOfView; }
		// UE: SetNearClipPlane / SetFarClipPlane
		void SetNearClipPlane(float InNearZ) { NearZ = InNearZ; }
		float GetNearClipPlane() const { return NearZ; }
		void SetFarClipPlane(float InFarZ) { FarZ = InFarZ; }
		float GetFarClipPlane() const { return FarZ; }

		void GetCameraView(FMinimalViewInfo& OutView) const;
	protected:
		// UE: 相机没有渲染代理，注册无场景操作。保留空实现让 CWorld 的
		// 多态注册循环照常分发（将来 per-view 数据注册在这里挂）。
		virtual void RegisterComponentWithWorld(CWorld* InWorld) override;

	private:
		float FieldOfView = 90.0f;   // UE: UCameraComponent::FieldOfView
		float NearZ = 10.0f;         // UE: NearClipPlane
		float FarZ = 5000.0f;        // UE: FarClipPlane
		float AspectRatio = 0.0f;    // UE: AspectRatio（0 = 用视口宽高比）
	};
}
