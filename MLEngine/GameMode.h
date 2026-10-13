#pragma once
#include "Refcount.h"
namespace ML
{
	class MLENGINE_API CGameMode : public CRefcount
	{
	public:
		CGameMode();
		virtual ~CGameMode();
	};
}
