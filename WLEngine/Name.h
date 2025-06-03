#pragma once
#include "NonCopyable.h"
#include "Common.h"

namespace WL
{
	class CName : public CNonCopyable
	{
	public:
		CName() {}
		CName(const std::string& name);
		bool operator == (const CName& name) const;
		bool operator == (const std::string& name) const;
		bool operator == (const char* pName) const;
		INT32 getOrder() const;

	private:
		static INT32 _calculateOrder(CName& name);

	private:
		INT32 mOrder = -1;
		std::string mName = "";
		static std::map<std::string, INT32>	SNameMap;
	};
}