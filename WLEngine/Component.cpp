#include "Component.h"

namespace WL
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


}