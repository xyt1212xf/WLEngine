#include "Renderer.h"
#include "MLEngine.h"
#include "TimerClock.h"
#include "GraphicPlug.h"
#include "DX12Device.h"

namespace ML
{
	CRenderer::CRenderer(CGraphicPlug* plug)
	: mpPlug(plug)
	{

	}

	CRenderer::~CRenderer()
	{

	}

	void CRenderer::_Draw()
	{
		static CTimerClock time;
		while (GEngine->IsRun())
		{
			time.begin();
			if (_Begin())
			{
				_Flush();
			}
			_End();
			double costTime = time.getTimerSecond();
		}
	}

	bool CRenderer::_Begin()
	{
		mpPlug->GetDevice()->BeginDraw();	
		return true;
	}

	void CRenderer::_Flush()
	{

	}

	void CRenderer::_End()
	{

	}

}
