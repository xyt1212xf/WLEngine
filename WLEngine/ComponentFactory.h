#pragma once
#include "TSingle.h"

namespace WL
{
	class CComponent;
	class CComponentFactory : public TSingle<CComponentFactory>
	{
	public:
		CComponent* CreateComponent(const std::string& szName);
	};
}