#pragma once
#include "Componet.h"

namespace WL
{
	class CDrawComponet : public CComponet
	{
	public:
		CDrawComponet();
		virtual ~CDrawComponet();
		virtual void draw(INT32 nTime);
	};
}