#pragma once
#include "LightComponentBase.h"
namespace ML
{
	class CSkyLightSceneProxy;
	class MLENGINE_API CSkyLightComponent : public CLightComponentBase
	{
		friend class CScene; 
	public:
		CSkyLightComponent() = default;
		~CSkyLightComponent();

		void SetIntensity(float InIntensity) { Intensity = InIntensity; }
		void SetLightColor(const Vec3F& InColor) { LightColor = InColor; }


		virtual CSkyLightSceneProxy* CreateSceneProxy();

	protected:
		virtual void RegisterComponentWithWorld(CWorld* InWorld) override;
		void SetSceneProxy(CSkyLightSceneProxy* InProxy) { SceneProxy = InProxy; }
		
	public:
		enum class ESkyLightSourceType : uint8
		{
			CapturedScene,     // UE: SLS_CapturedScene
			SpecifiedCubemap,  // UE: SLS_SpecifiedCubemap
		};
		CSkyLightSceneProxy* SceneProxy = nullptr;

		ESkyLightSourceType SourceType = ESkyLightSourceType::CapturedScene;
		float SourceCubemapAngle = 0.0f;         // UE: SourceCubemapAngle
		float SkyDistanceThreshold = 150000.0f;  // UE: SkyDistanceThreshold
		float BlendDistance = 0.0f;              // UE: BlendDistance
		bool  bRealTimeCapture = false;          // UE: bRealTimeCapture
	};
}
