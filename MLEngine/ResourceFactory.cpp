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

	CStreamableRenderAsset* CResourceFactory::createResource(ResourceType type)
	{
		CStreamableRenderAsset* pResource = nullptr;
		switch (type)
		{
		case ResourceType::Mesh:
		{
//			pResource = new CMeshResource();
//			pResource->mTypeName = "Mesh";
			break;
		}
		}
		return pResource;
	}

}
