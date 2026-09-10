#include "UIAnimation.h"
#include "FrameAniComponet.h"
#include "Foundation.h"
namespace WL
{

	CUIAnimation::CUIAnimation()
	{
		mWndType = ui_animation;
		mpAnimationCmp = WL_NEW(CFrameAniComponet, Component);
	}

	CUIAnimation::~CUIAnimation()
	{
		WL_DELETE(mpAnimationCmp, Component);
	}

	void CUIAnimation::setTexture(const std::string& szFile)
	{
		if (szFile.length() > 0)
		{
			auto file = std::move(Foundation::splitString<std::string>(szFile, " "));
			mMaxFrameCount = std::stoi(file[0]);
			loadAnimation(file[1], mMaxFrameCount);
		}
	}

	void CUIAnimation::setAniLoop(bool bLoop)
	{
		mbLoop = bLoop;
	}

	bool CUIAnimation::isAniLoop() const
	{
		return	mbLoop;
	}

	bool CUIAnimation::setCurrentFrame(int nNumber)
	{
		bool bResult = false;
		if (mpAnimationCmp)
		{
			bResult = mpAnimationCmp->gotoFrame(nNumber);
			if (bResult)
			{
				mCurrentFrame = nNumber;
			}
		}
		return bResult;
	}

	int CUIAnimation::getCurrentFrame() const
	{
		return mCurrentFrame;
	}


	bool CUIAnimation::loadAnimation(const std::string& szFile, int nCount, bool bAsynch /*= true*/)
	{
		return mpAnimationCmp->loadFrame(szFile, nCount, bAsynch);
	}

	void CUIAnimation::setAniSpeed(float fSpeed)
	{
		mSpeed = fSpeed;
	}

	float CUIAnimation::getAniSpeed() const
	{
		return mSpeed;
	}

	void CUIAnimation::_tick(UINT32 dt)
	{
		mCurrentFrame += mSpeed;
		if (!mbLoop)
		{
			if (mCurrentFrame > mMaxFrameCount - 1)
			{
				mCurrentFrame = mMaxFrameCount - 1;
			}
		}

		int nFrame = mCurrentFrame;

		auto pTexture = mpAnimationCmp->getFrameTexture(nFrame % mMaxFrameCount);
		if (nullptr != pTexture)
		{
			mTextureMap[TextureUseType::Base] = pTexture;
		}
		
	}
}