#pragma once
#include "Component.h"
#include "Vector.h"
#include "Matrix.h"

namespace WL
{
	class CTexture;
	class CFrameAniComponet : public CComponent
	{
	public:
		CFrameAniComponet();
		virtual ~CFrameAniComponet();
		void play();
		void stop();
		void resume();
		int getFrameCount();
		int getCurrentFrame();
		bool gotoFrame(int index);
		bool loadFrame(const std::string& szFile, int nCount = 1, bool bAsynch = true);
		CTexture* getFrameTexture(int nIndex) const;

	private:
		std::vector<CTexture*>	mImage;
		int mMaxFrame = 1;
		int mCurrentFrame = 0;
	};
}