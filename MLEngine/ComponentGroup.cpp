#include "ComponentGroup.h"
#include "Component.h"

namespace WL
{
	SComponentGrounp::~SComponentGrounp()
	{
		for (auto item : mComponent)
		{
			WL_DELETE(item.second, Component);
		}
		mComponent.clear();
	}

	void SComponentGrounp::addComponent(CComponent* pComponent)
	{
		if (nullptr != pComponent)
		{
			auto szName = pComponent->getComponentName();
			auto [iter, inserted] = mComponent.try_emplace(szName, pComponent);
			if (!inserted)
			{
				SafeDelete(iter->second);
				iter->second = pComponent;
			}
		}
	}

	CComponent* SComponentGrounp::getComponetByName(const std::string& szName)
	{
		CComponent* pComponet = nullptr;
		auto iter = mComponent.find(szName);
		if (mComponent.end() != iter)
		{
			pComponet = iter->second;
		}
		return pComponet;

	}

	void SComponentGrounp::removeComponent(CComponent* pComponent)
	{
		auto iter = mComponent.begin();
		auto iterEnd = mComponent.end();
		while (iter != iterEnd)
		{
			if (pComponent == iter->second)
			{
				WL_DELETE(iter->second, Component);
				mComponent.erase(iter);
				break;
			}
			else
			{
				++iter;
			}
		}
	}


}