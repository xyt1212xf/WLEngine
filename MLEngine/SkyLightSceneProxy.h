#pragma once
#include "Common.h"
#include "Vector.h"

namespace ML
{
	class MLENGINE_API CSkyLightSceneProxy
	{
	public:
		struct FInitParams
		{
			uint32    Id = 0;
			Vec4F     LightColor = Vec4F(1.0f, 1.0f, 1.0f, 1.0f);   // rgb = color * intensity（游戏线程已合并）
			Vec4F     IrradianceSH[9];                              // UE: IrradianceEnvironmentMap (FSHVectorRGB3)
		//	CTexture* Cubemap = nullptr;                            // UE: ProcessedTexture
			float     AverageBrightness = 1.0f;                     // UE: AverageBrightness
			float     BlendFraction = 1.0f;                         // UE: BlendFraction
			bool      bRealTimeCaptureEnabled = false;              // UE: bRealTimeCaptureEnabled
			Vec3F     CapturePosition = Vec3F(0.f, 0.f, 0.f);       // UE: CapturePosition
			bool      bVisible = true;
		};

		explicit CSkyLightSceneProxy(const FInitParams& InParams);

		// 所有权在组件（UE: USkyLightComponent 创建并在 DestroyRenderState 时
		// 延迟删除）；场景经 DisableSkyLight 只摘链、不 delete。
		~CSkyLightSceneProxy() = default;

	private:
		uint32 GetId() const { return Id; }
		bool   IsSkyLightValid() const { return bVisible; }
		bool   IsMovable() const { return true; }               // UE: FSkyLightSceneProxy::IsMovable

		// UE: FSkyLightSceneProxy::GetEffectiveLightColor —— rgb 已含强度
		const Vec4F& GetEffectiveLightColor() const { return LightColor; }

		void SetLightColor(const Vec4F& InColor) { LightColor = InColor; }
		void SetVisibility(bool bInVisible) { bVisible = bInVisible; }
		void SetIrradianceSH(const Vec4F InSH[9])
		{
			for (int32 i = 0; i < 9; ++i)
			{
				IrradianceSH[i] = InSH[i];
			}
		}

	private:
		uint32    Id = 0;
		Vec4F     LightColor = Vec4F(1.0f, 1.0f, 1.0f, 1.0f);
		Vec4F     IrradianceSH[9];
		//CTexture* Cubemap = nullptr;
		float     AverageBrightness = 1.0f;
		float     BlendFraction = 1.0f;
		bool      bRealTimeCaptureEnabled = false;
		Vec3F     CapturePosition;
		bool      bVisible = true;

	};
}
