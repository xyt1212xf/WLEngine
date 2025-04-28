#include "FrameAniComponet.h"
#include "Texture.h"
#include "GuiEngine.h"

namespace WL
{

	CFrameAniComponet::CFrameAniComponet()
	{
		mComponentName = "FrameAniComponet";
	}

	CFrameAniComponet::~CFrameAniComponet()
	{

	}

	void CFrameAniComponet::play()
	{

	}

	void CFrameAniComponet::stop()
	{

	}

	void CFrameAniComponet::resume()
	{

	}

	int CFrameAniComponet::getFrameCount()
	{
		return -1;
	}

	int CFrameAniComponet::getCurrentFrame()
	{
		return -1;
	}

	bool CFrameAniComponet::gotoFrame(int index)
	{
		if (index < mImage.size())
		{
			mCurrentFrame = index;
			return true;
		}
		else
		{
			return false;
		}
	}


	bool CFrameAniComponet::loadFrame(const std::string& szFile, int nCount /*= 1*/, bool bAsynch /*= true*/)
	{
		bool bSuccessed = true;
		for (auto item : mImage)
		{
			WL_DECREASE(item);
		}
		mImage.clear();
		mImage.reserve(nCount);

		for (int i = 0; i < nCount; ++i)
		{
			std::string szFrame = szFile + std::to_string(i) + "_0.png";
			auto pTexture = CGuiEngine::loadTexture(szFrame);
			if (nullptr != pTexture)
			{
				mImage.push_back(pTexture);
			}
			else
			{
				bSuccessed = false;
				break;
			}
		}
		return bSuccessed;
	}
	
	CTexture* CFrameAniComponet::getFrameTexture(int nIndex) const
	{
		if( nIndex < mImage.size() )
		{
			return mImage[nIndex];
		}
		else
		{
			return nullptr;
		}
	}

}