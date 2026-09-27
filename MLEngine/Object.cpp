#include "Object.h"
namespace ML
{

	CObject::CObject(const std::string& InName)
	: Name(InName)
	{

	}

	CObject::CObject(std::string&& InName) noexcept
	{
		Name = std::move(InName);
	}

	const std::string& CObject::GetName() const
	{
		return Name;
	}

}