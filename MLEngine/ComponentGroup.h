#pragma once
#include "Common.h"
#include "MemoryMgr.h"

namespace WL
{
	class CComponent;
	struct WLENGINE_API SComponentGrounp
	{
		//////////////////////////////////////////////////////////////////////////
		//Function
		SComponentGrounp() = default;
		virtual ~SComponentGrounp();

		void addComponent(CComponent* pComponent);

		template<typename T, typename... Rest>
		void addComponent()
		{
			static_assert(std::is_base_of<CComponent, T>::value, "T must inherit from CComponent");
			auto pComponent = WL_NEW(T, Component);
			addComponent(pComponent);
			if constexpr (sizeof...(Rest) > 0)
			{
				// 使用模板展开调用单模板版本
				(addComponent<Rest>(), ...);
			}
		}

		void removeComponent(CComponent* pComponent);

		template<typename T, typename... Rest>
		void removeComponent()
		{
			auto iter = mComponent.begin();
			auto iterEnd = mComponent.end();
			while (iter != iterEnd)
			{
				if (nullptr != dynamic_cast<T*>(iter->second))
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
			if constexpr (sizeof...(Rest) > 0)
			{
				// 使用模板展开调用单模板版本
				(removeComponent<Rest>(), ...);
			}
		}

		template< typename T>
		T* getComponent()
		{
			T* pComponent = nullptr;
			for (auto item : mComponent)
			{
				pComponent = dynamic_cast<T*>(item.second);
				if (nullptr != pComponent)
				{
					break;
				}
			}
			return pComponent;
		}

		CComponent* getComponetByName(const std::string& szName);

		//////////////////////////////////////////////////////////////////////////
		//Data
		std::map<std::string, CComponent*> mComponent;
	};

}