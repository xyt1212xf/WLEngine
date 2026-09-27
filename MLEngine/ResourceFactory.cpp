#include "ResourceFactory.h"
#include "MLEngine.h"
namespace ML
{

	CResourceFactory::CResourceFactory()
	{

	}

	CResourceFactory::~CResourceFactory()
	{

	}

	bool CResourceFactory::LoadResource(const std::string& resourceName)
	{
		return true;
	}

	CResource* CResourceFactory::createResource(ResourceType type)
	{
		CResource* pResource = nullptr;
		switch (type)
		{
		case ResourceType::Mesh:
		{
			pResource = new CMesh();
			pResource->mTypeName = "Mesh";
			break;
		}
		return pResource;
		}
	}

}
