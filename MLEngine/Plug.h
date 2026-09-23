#pragma once
#include "TSingle.h"

namespace ML
{
	class CPlug : public TSingle<CPlug>
	{
	public:
		virtual bool Initialise();
		virtual bool UnInitialise();
	};
}