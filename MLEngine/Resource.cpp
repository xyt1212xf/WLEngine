#include "Resource.h"
#include "MLEngine.h"
namespace ML
{
	CResource::CResource()
	{
	}

	CResource::~CResource()
	{
	
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

	ResourceStatus CResource::GetStatus() const
	{
		return mStatus;
	}

	ResourceType CResource::getType()
	{
		return sResType;
	}

	std::string CResource::getTypeName() const
	{
		return mTypeName;
	}

	bool CResource::initialise()
	{
		return false;
	}

	void CResource::SetStatus(ResourceStatus status)
	{
		mStatus = status;
	}

}