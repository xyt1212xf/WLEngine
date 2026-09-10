#pragma once
#include "CallBack.h"
#include "EngineBase.h"

namespace WL
{
	class CFont;
	class CFontEngine :virtual public CEngineBase
	{
	public:
		CFontEngine();
		virtual ~CFontEngine();
		bool initialize();
		void unInitialize();
		void draw(CFont* pFont, float x, float y, const std::string& szContent, const Vec4F& color);
	};
}