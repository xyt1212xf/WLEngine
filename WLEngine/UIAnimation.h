#pragma once
#include "UIWnd.h"
namespace WL
{
	class CFrameAniComponet;
	class CUIAnimation : public CUIWnd
	{
	public:
		CUIAnimation();
		virtual ~CUIAnimation();
		virtual void setTexture(const std::string& szFile) final;
		virtual void setAniLoop(bool bLoop) final;
		virtual bool isAniLoop() const final;
		virtual bool setCurrentFrame(int nNumber) final;
		virtual int getCurrentFrame() const final;
		virtual void setAniSpeed(float fSpeed) final;
		virtual float getAniSpeed() const final;

		bool loadAnimation(const std::string& szFile, int nCount, bool bAsynch = true);
		

	protected:
		virtual void _tick(UINT32 dt) final;

	private:
		bool mbLoop = true;
		CFrameAniComponet* mpAnimationCmp = nullptr;
		float mCurrentFrame = 0;
		int mMaxFrameCount = 1;
		float mSpeed = 0.27f;
	};
}