#pragma once
#include <string>
#include "Resource.h"

namespace ML
{
	class MLENGINE_API CResourceFactory 
	{
	public:
		CResourceFactory();
		virtual ~CResourceFactory();

		template<typename T>
		T* CreateResource();

		bool LoadResource(const std::string& resourceName);

	private:
		CResource* createResource(ResourceType type);
	};

	template<typename T>
	T* CResourceFactory::CreateResource()
	{
		return createResource(T::type);
	}

}