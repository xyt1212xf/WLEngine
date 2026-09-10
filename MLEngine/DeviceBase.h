#pragma once
#include "Refcount.h"
#include "Common.h"
#include "FoundationStruct.h"
#include "RenderStateHeader.h"


namespace WL
{
	struct SRenderState;
	struct SViewPort
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
		virtual SRenderState* createRenderState(SRenderStateDesc& renderStateDesc);

	protected:
		SWindowConfig* mpConfig = nullptr;
		CRenderStateMgr* mpRenderStateMgr = nullptr;
		RectI	mRT = {0,0,0,0};
	};
}