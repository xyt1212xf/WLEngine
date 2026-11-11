#include "Object.h"
namespace ML
{

	CObject::CObject(const std::string& InName)
	: mName(InName)
	{

	}

	CObject::CObject(std::string&& InName) noexcept
	{
		mName = std::move(InName);
	}

	const std::string& CObject::GetName() const
	{
		return mName;
	}

	void CObject::SetName(const std::string& InName)
	{
		mName = InName;
	}

}
