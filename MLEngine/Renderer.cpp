#include "Renderer.h"
#include "MLEngine.h"
#include "TimerClock.h"
namespace ML
{
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
		return true;
	}

	void CRenderer::_Flush()
	{

	}

	void CRenderer::_End()
	{

	}

}
