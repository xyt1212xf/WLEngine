#pragma once
#include "Message.h"

namespace WL
{
	struct SController
	{
		virtual bool onEvent(SEvent& ) { return false; }
	};
}
