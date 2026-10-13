#pragma once
#include "Common.h"

namespace ML
{
	enum  RendererType
	{
		Renderer = 0,
		CustomRenderer = 99,
		ShadowRenderer = 100,
		FrontRenderer = 200,
		DeferredRenderer = 300,
		GuiRenderer = 400,
	};

	enum  DrawQualityLevel
	{
		Low,
		Middle,
		Hihg,
	};

	class CPipelineState;

	class MLENGINE_API CRenderer
	{
		friend class CGraphicPlug;
	public:
		CRenderer(CGraphicPlug* plug);
		virtual ~CRenderer();
		
	private:
		void _Draw();
		bool _Begin();
		void _Flush();
		void _End();

	private:
		CGraphicPlug*	mpGraphicPlug = nullptr;
		// Solid-color pipeline for the first bring-up (created lazily in _Flush).
		CPipelineState*	mpSolidColorPSO = nullptr;
	};
}
