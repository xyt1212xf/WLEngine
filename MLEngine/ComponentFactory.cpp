#include "ComponentFactory.h"
#include "Transform.h"
#include "AbilitySystemComponent.h"
#include "FrameAniComponet.h"
#include "Collision.h"
#include "SkeletonAniComponent.h"

namespace WL
{

	CComponent* CComponentFactory::CreateComponent(const std::string& szName)
	{
		CComponent* pComponet = nullptr;
		if ("AbilitySystemComponent" == szName)
		{
			pComponet = WL_NEW(CAbilitySystemComponent, Component);
		}
		else if ("TranformComponet" == szName)
		{
			return WL_NEW(CTranformComponet, Component);
		}
		else if ("Collision" == szName)
		{
			return WL_NEW(CCollision, Component);
		}
		return pComponet;
	}

}