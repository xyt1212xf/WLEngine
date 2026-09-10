#pragma once
#include "Refcount.h"
namespace WL
{
	class CRenderTargetView : public CRefcount
	{
	public:
		CRenderTargetView();
		virtual ~CRenderTargetView();
	};
}