#pragma once
#include "LightComponentBase.h"
#include "LightSceneProxy.h" 
namespace ML
{
	class MLENGINE_API CLightComponent : public CLightComponentBase
	{
	public:
	    CLightComponent();
		virtual ~CLightComponent();
		virtual CLightSceneProxy* CreateSceneProxy() { return nullptr; }

	};
}
