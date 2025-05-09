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

	//////////////////////////////////////////////////////////////////////////
	ComponentGrounp::~ComponentGrounp()
	{
		for( auto item : mComponent )
		{
			WL_DELETE(item.second, Component);
		}
		mComponent.clear();
	}

	CComponent* ComponentGrounp::getComponetByName(const std::string& szName)
	{
		CComponent* pComponet = nullptr;
		auto iter = mComponent.find(szName);
		if (mComponent.end() != iter)
		{
			pComponet = iter->second;
		}
		return pComponet;

	}



}