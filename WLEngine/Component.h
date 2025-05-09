#pragma once
#include "Common.h"
#include "MemoryMgr.h"

namespace WL
{
	class WLENGINE_API CComponent
	{
	public:
		CComponent() = default;
		virtual ~CComponent();
		const std::string& getComponentName() const;
		void setComponentName(const std::string& szName);
		virtual void tick([[maybe_unused]] UINT32 dt){};
		
	protected:
		std::string mComponentName = "Component";
	};


	//////////////////////////////////////////////////////////////////////////
	struct WLENGINE_API ComponentGrounp
	{
		//////////////////////////////////////////////////////////////////////////
		//Function
		ComponentGrounp() = default;
		virtual ~ComponentGrounp();

		void addComponent(CComponent* pComponent)
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

		void removeComponent(CComponent* pComponent)
		{
			auto iter = mComponent.begin();
			auto iterEnd = mComponent.end();
			while (iter != iterEnd)
			{
				if (pComponent == iter->second )
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