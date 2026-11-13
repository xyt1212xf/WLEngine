#pragma once
#include "LightShader.h"
#include "Matrix.h"

namespace ML
{
	//====================================================================
	// UE: FLightSceneProxy (Engine/Source/Runtime/Renderer/Public/LightSceneProxy.h)
	//
	// 渲染线程侧的一盏灯：游戏线程在 CLightComponent::CreateSceneProxy() 时
	// 快照构造，之后**只有渲染线程**读写；游戏线程的修改全部经
	// FRenderCommandQueue 投递（与 FPrimitiveSceneProxy 同一套纪律）。
	//
	// 方向约定：UE 的灯 Forward 是 +X，本引擎是 +Z（LH / 行向量，见
	// CSceneComponent::GetWorldMatrix）。语义与 UE 一致（Direction = 光的照射
	// 方向），只是轴向沿用引擎既有约定，着色器按需取反。
	//====================================================================
	class MLENGINE_API CLightSceneProxy
	{
		friend class CScene;
	public:
		struct FInitParams
		{
			uint32        Id = 0;
			ELightGPUType Type = ELightGPUType::Directional;
			Vec4F         ColorIntensity = Vec4F(1.0f, 1.0f, 1.0f, 0.0f); // rgb = color * intensity
			FMatrix44     WorldMatrix = FMatrix44::identity();
			float         Radius = 0.0f;              // UE: UPointLightComponent::AttenuationRadius
			float         FalloffExponent = 2.0f;     // UE: LightFalloffExponent
			bool          bVisible = true;
		};

		explicit CLightSceneProxy(const FInitParams& InParams);

        ~CLightSceneProxy() = delete;

	private:
		void UpdateTransformDerived(const FMatrix44& InWorldMatrix);
		void PackGPULight(FSceneLightGPU& OutLight) const;

		uint32 GetId() const { return Id; }
		ELightGPUType GetLightType() const { return Type; }
		bool IsVisible() const { return bVisible; }
		// UE: FLightSceneProxy::GetDirection() —— 光**照射**的方向（组件 Forward）。
		const Vec3F& GetDirection() const { return Direction; }
		float GetRadius() const { return Radius; }

		// UE: FScene::UpdateLightTransform -> FLightSceneProxy::SetTransform
		void SetTransform(const FMatrix44& InWorldMatrix) { UpdateTransformDerived(InWorldMatrix); }
		// UE: FLightSceneProxy::SetColor
		void SetColorIntensity(const Vec4F& InColorIntensity) { ColorIntensity = InColorIntensity; }
		// UE: FLightSceneInfo::SetVisibility
		void SetVisibility(bool bInVisible) { bVisible = bInVisible; }

	private:
		uint32        Id = 0;
		ELightGPUType Type = ELightGPUType::Directional;
		Vec4F         ColorIntensity = Vec4F(1.0f, 1.0f, 1.0f, 0.0f);
		FMatrix44     WorldMatrix;
		Vec3F         Direction = Vec3F(0.0f, 0.0f, 1.0f);   // 照射方向
		Vec3F         Position;
		float         Radius = 0.0f;
		float         FalloffExponent = 2.0f;
		bool          bVisible = true;
	};
}
