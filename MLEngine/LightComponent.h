#pragma once
#include "LightComponentBase.h"
namespace ML
{
	class CWorld;
	class CLightSceneProxy;
	class MLENGINE_API CLightComponent : public CLightComponentBase
	{
		friend class CScene; 
	public:
	    CLightComponent();
		virtual ~CLightComponent();
		virtual CLightSceneProxy* CreateSceneProxy() { return nullptr; }
		CLightSceneProxy* GetSceneProxy() const { return SceneProxy; }

	protected:
		virtual void RegisterComponentWithWorld(CWorld* InWorld) override;
		void SetSceneProxy(CLightSceneProxy* InProxy) { SceneProxy = InProxy; }

	protected:
		CLightSceneProxy* SceneProxy = nullptr;
	};
}
