#pragma once
#include "Common.h"
#include "ResourceFactory.h"
namespace ML
{
	class CResourceMgr
	{
	public:
		CResourceMgr();
		virtual ~CResourceMgr();

		bool initialize();
		void unInitialize();
		void releaseResource();
		template<typename T>
		T* createResource(ResourceType type)noexcept
		{
			return nullptr;
			//try
			//{
			//	CResource* pResource = mpFactory->createResource(type);
			//	T* p = dynamic_cast<T*>(pResource);
			//	if (p != nullptr)
			//	{
			//		UINT32 uuid = pResource->getUUID();
			//		mResourceMap[uuid] = pResource;
			//		return p;
			//	}
			//	else
			//	{
			//		throw(0);
			//	}
			//}
			//catch (...)
			//{
			//	return nullptr;
			//}
		}
	private:
	};

}