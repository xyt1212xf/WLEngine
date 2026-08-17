#include "Object.h"
namespace WL
{

	CObject::CObject(const std::string& InName)
	: Name(InName)
	{

	}

	CObject::CObject(std::string&& InName) noexcept
	{
		Name = std::move(InName);
	}

	void CObject::AddReference(CObject* Obj)
	{
		if (Obj) 
		{
			References.push_back(Obj);
		}
	}
}