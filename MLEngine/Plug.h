#pragma once
#include "TSingle.h"

namespace ML
{
	class MLENGINE_API CPlug : public TSingle<CPlug>
	{
	public:
		virtual bool Initialise();
		virtual bool UnInitialise();
	};
}