#pragma once
#include "RenderStateHeader.h"
#include "Vector.h"
#include "RenderTargetView.h"
namespace WL
{
	class CRenderContext
	{
	public:
		CRenderContext();
		virtual ~CRenderContext();

	protected:
		CVec4 mColor;
	};
}