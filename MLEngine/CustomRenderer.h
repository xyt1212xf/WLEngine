#pragma once
#include "Renderer.h"
#include "ActorEntity.h"

namespace WL
{
	class CCustomRenderer : public CRenderer
	{
		typedef CRenderer Parent;
	public:
		CCustomRenderer();
		virtual ~CCustomRenderer();
	};
};