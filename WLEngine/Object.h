#pragma once
#include "Common.h"
#include "Refcount.h"
namespace WL
{
	class  CObject : public CRefcount
	{
	private:
		std::string Name;
		bool bMarked = false;         
	};
}