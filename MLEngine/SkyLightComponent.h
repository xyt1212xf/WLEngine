#pragma once
#include "LightComponentBase.h"
namespace ML
{
	class CSkyLightSceneProxy;
	class MLENGINE_API CSkyLightComponent : public CLightComponentBase
	{
	public:
		CSkyLightComponent() = default;
		~CSkyLightComponent() = default;

		void SetIntensity(float InIntensity) { Intensity = InIntensity; }
		void SetLightColor(const Vec3F& InColor) { LightColor = InColor; }

		virtual CSkyLightSceneProxy* CreateSceneProxy();

	public:
		enum class ESkyLightSourceType : uint8
		{
			CapturedScene,     // UE: SLS_CapturedScene
			SpecifiedCubemap,  // UE: SLS_SpecifiedCubemap
		};

		ESkyLightSourceType SourceType = ESkyLightSourceType::CapturedScene;
		float SourceCubemapAngle = 0.0f;         // UE: SourceCubemapAngle
		float SkyDistanceThreshold = 150000.0f;  // UE: SkyDistanceThreshold
		float BlendDistance = 0.0f;              // UE: BlendDistance
		bool  bRealTimeCapture = false;          // UE: bRealTimeCapture
	};
}
