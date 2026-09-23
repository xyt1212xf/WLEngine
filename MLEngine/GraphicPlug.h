#pragma once
#include "Plug.h"

namespace ML
{
	class MLENGINE_API CGraphicPlug : public CPlug
	{
	public:
		virtual bool Initialise() override final;
		virtual bool UnInitialise()override final;
	};
}
