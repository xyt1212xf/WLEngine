#pragma once
#include "Renderer.h"
#include "ShadowEffect.h"

namespace WL
{
	class CLightRenderer : public CRenderer
	{
		typedef CRenderer Parent;
	public:
		CLightRenderer();
		virtual ~CLightRenderer();
	};
}