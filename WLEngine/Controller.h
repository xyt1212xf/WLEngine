#pragma once
#include "Message.h"

namespace WL
{
	struct CController
	{
		virtual bool onEvent(event& ) { return false; }
	};
}
