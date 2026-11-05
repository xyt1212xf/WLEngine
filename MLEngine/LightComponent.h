#pragma once
#include "SceneComponent.h"
namespace ML
{
	class CLightComponent : public CSceneComponent
	{
	public:
	    CLightComponent();
		bool IsVisible() const { return bVisible; }
		void SetVisibility(bool b) { bVisible = b; }  // 变化时 PushSceneCommand
		Vec3F GetColorIntensity() const { return LightColor * Intensity; } // GPU 侧合并

	protected:
		bool bVisible = true;
		Vec3F LightColor = Vec3F(1, 1, 1);
		float Intensity = 1.0f;
	};
}
