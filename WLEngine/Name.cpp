#include "Name.h"
namespace WL
{
	INT32 CName::_calculateOrder(CName& name)
	{
		static INT32 nOrder = -1;
		auto [iter, insert] = SNameMap.try_emplace(name.mName, ++nOrder);
		if (insert)
		{
			(iter->second) = nOrder;
		}
		else
		{
			--nOrder;
		}
		name.mOrder = nOrder = (iter->second);
		return (iter->second);
	}

	std::map<std::string, INT32> CName::SNameMap;
	CName::CName(const std::string& name)
	{
		mName = name;
		_calculateOrder(*this);
	}

	INT32 CName::getOrder() const
	{
		return mOrder;
	}

	bool CName::operator==(const char* pName) const
	{
		return 0 == mName.compare(pName);
	}

	bool CName::operator==(const std::string& name) const
	{
		return mName == name;
	}

	bool CName::operator==(const CName& name) const
	{
		return mOrder == name.mOrder;
	}

}