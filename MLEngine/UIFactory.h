#pragma once
#include "TSingle.h"
#include "UIHeader.h"

namespace WL
{
	class CUIWnd;
	class CUIFactory : public TSingle<CUIFactory>
	{
	public:
		CUIWnd* createUI(WndType type);
	};
}