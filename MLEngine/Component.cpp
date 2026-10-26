#include "Component.h"

namespace ML
{
	CComponent::~CComponent()
	{

	}

	const std::string& CComponent::getComponentName() const
	{
		return mComponentName;
	}

	void CComponent::setComponentName(const std::string& szName)
	{
		mComponentName = szName;
	}

	void CComponent::RegisterComponentWithWorld([[maybe_unused]] CWorld* InWorld)
	{
		// UE: UActorComponent::RegisterComponentWithWorld, base behaviour.
		// Pure logic components have no scene state to register - nothing to do.
	}

}
