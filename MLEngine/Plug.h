#pragma once
#include "TSingle.h"

namespace ML
{
	class MLENGINE_API CPlug : public TSingle<CPlug>
	{
	public:
		virtual ~CPlug() = default;
		virtual bool Initialise();
		virtual bool UnInitialise();
		virtual void Process(int32 DeltaSeconds) const;
	};
}