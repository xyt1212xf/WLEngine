#pragma once
#include "Refcount.h"
namespace ML
{
	class CGameMode : public CRefcount
	{
	public:
		CGameMode();
		virtual ~CGameMode();
	};
}