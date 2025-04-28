#pragma once
#include "Refcount.h"
#include "Common.h"
#include "FoundationStruct.h"
#include "RenderStateHeader.h"


namespace WL
{
	struct RenderState;
	struct ViewPort
	{
		float TopLeftX;
		float TopLeftY;
		float Width;
		float Height;
		float MinDepth;
		float MaxDepth;
	};

	class CRenderStateMgr;
	class CDeviceBase : public CRefcount
	{
	public:
		CDeviceBase();
		virtual ~CDeviceBase();
		virtual RenderState* createRenderState(RenderStateDesc& renderStateDesc);

	protected:
		WindowConfig* mpConfig = nullptr;
		CRenderStateMgr* mpRenderStateMgr = nullptr;
		RectI	mRT = {0,0,0,0};
	};
}