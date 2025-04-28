#include "Resource.h"
#include "WLEngine.h"

namespace WL
{
	UINT32 CResource::gUUID = 0;
	CResource::CResource()
	{
		mUUID = ++gUUID;
	}

	CResource::~CResource()
	{
		if (nullptr != GEngine)
		{
			GEngine->removeResource(this);
		}
	}

	UINT32 CResource::getUUID()
	{
		return mUUID;
	}

	void CResource::setResourceName(const std::string& szResourceName)
	{
		mResourceName = szResourceName;
	}

	const std::string& CResource::getResourceName() const
	{
		return mResourceName;
	}

	void CResource::setName(const std::string& szName)
	{
		mName = szName;
	}

	const std::string& CResource::getName() const
	{
		return mName;
	}

	ResourceType CResource::getType()
	{
		return mType;
	}

	std::string CResource::getTypeName() const
	{
		return mTypeName;
	}

	bool CResource::initialise()
	{
		return false;
	}

}