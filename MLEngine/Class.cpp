
#include "Class.h"
namespace WL
{

	bool CClass::IsChildOfWalk(const StructInfo* Other) const
	{
		for (const StructInfo* Current = SuperStruct; Current != nullptr; Current = Current->SuperStruct)
		{
			if (Current == Other)
			{
				return true;
			}
		}
		return false;
	}

	bool CClass::IsChildOfArray(const StructInfo* Other) const
	{
		for (const StructInfo* Parent : InheritanceCache)
		{
			if (Parent == Other)
			{
				return true;
			}
		}
		return false;
	}

	void CClass::BuildInheritanceCache()
	{

	}

}