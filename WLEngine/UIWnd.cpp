#include "UIWnd.h"
#include "Texture.h"
#include "WLEngine.h"

namespace WL
{

	CUIWnd::CUIWnd()
	{
		mData.mArea[0].uv = { 0, 0};
		mData.mArea[0].color = { 1,1,1,1 };

		mData.mArea[1].uv = { 1, 0 };
		mData.mArea[1].color = { 1,1,1,1 };

		mData.mArea[2].uv = { 0, 1 };
		mData.mArea[2].color = { 1,1,1,1 };

		mData.mArea[3].uv = { 1, 1 };
		mData.mArea[3].color = { 1,1,1,1 };
	}

	CUIWnd::~CUIWnd()
	{
		auto pTexture = mTextureMap[TextureUseType::Base];
		WL_DECREASE(pTexture);
		mTextureMap.clear();
	}

	WndType CUIWnd::getWndType() const
	{
		return mWndType;
	}

	void CUIWnd::setPosition(const Vec3F& _pos)
	{	
		Vec3F pos = _pos;
		auto offset = pos - mPosition;
	
		mPosition += offset;
		pos = { pos.x - GEngine->getWindowConfigPtr()->nWidth * 0.5f,
			    GEngine->getWindowConfigPtr()->nHeight * 0.5f - pos.y,
				pos.z };

		mData.mArea[0].position = pos;

		mData.mArea[1].position = pos;
		mData.mArea[1].position.x = pos.x + mSize.x;

		mData.mArea[2].position = pos;
		mData.mArea[2].position.y = pos.y + mSize.y;

		mData.mArea[3].position.x = pos.x + mSize.x;
		mData.mArea[3].position.y = pos.y + mSize.y;

		mDrawPosition = pos;
		setSize(mSize);
		for (auto item : mChildren)
		{
			item->setPosition(item->getPosition() + offset);
		}
	}

	const Vec3F& CUIWnd::getPosition() const
	{
		return mPosition;
	}


	void CUIWnd::setSize(const Vec2F& size)
	{
		mSize = size;
		_updateArea();
	}

	void CUIWnd::_updateArea()
	{
		mData.mArea[1].position.x = mDrawPosition.x + mSize.x;

		mData.mArea[2].position.y = mDrawPosition.y - mSize.y;

		mData.mArea[3].position.x = mDrawPosition.x + mSize.x;
		mData.mArea[3].position.y = mDrawPosition.y - mSize.y;
	}

	const Vec2F& CUIWnd::getSize() const
	{
		return mSize;
	}

	void CUIWnd::setTexture(const std::string& szFile)
	{
		auto pTexture = CGuiEngine::loadTexture(szFile);
		if (nullptr != pTexture)
		{
			mTextureName = szFile;
			pTexture->addRef();
			WL_DECREASE(mTextureMap[TextureUseType::Base]);
			mTextureMap[TextureUseType::Base] = pTexture;	
		}
	}

	void CUIWnd::_tick(UINT32 dt)
	{

	}

	void CUIWnd::addChild(CUIWnd* pChild)
	{
		auto iter = std::find(mChildren.begin(), mChildren.end(), pChild);
		if (iter == mChildren.end())
		{
			mChildren.push_back(pChild);
			pChild->mpParentWnd = this;
			WL_INCREASE(pChild);
		}
		for( auto item : mChildren )
		{
			item->mpParentWnd = this;
			item->setPosition(item->getPosition() + mPosition);
		}
	}

	void CUIWnd::removeChild(CUIWnd* pChild)
	{
		auto iter = std::find(mChildren.begin(), mChildren.end(), pChild);
		if (iter != mChildren.end())
		{
			pChild->mpParentWnd = nullptr;
			mChildren.push_back(pChild);
			WL_DECREASE(pChild);
		}
	}

	void CUIWnd::setRotation(float fRadian)
	{
		mRadian = fRadian;
	}

	float CUIWnd::getRotation() const
	{
		return mRadian;
	}

	void CUIWnd::setId(UINT32 Id)
	{
		mId = Id;
	}

	UINT32 CUIWnd::getId()
	{
		return mId;
	}

	std::optional<CUIWnd*> CUIWnd::getchild(UINT32 id) const
	{
		for( auto item : mChildren )
		{
			if (item->getId() == id)
			{	
				return item;	
			}
		}
		return std::nullopt;
	}

	void CUIWnd::setVisible(bool bValue)
	{
		mVisible = bValue;
	}

	bool CUIWnd::isVisible() const
	{
		return mVisible;
	}

	bool CUIWnd::isXYAxisSwap()
	{
		return mXYAxisSwap;
	}

	void CUIWnd::setXYAxisSwap(bool bValue)
	{
		if (mXYAxisSwap != bValue)
		{
			mXYAxisSwap = bValue;
			std::swap(mData.mArea[0].uv, mData.mArea[1].uv);
			std::swap(mData.mArea[2].uv, mData.mArea[3].uv);
		}
	}

	void CUIWnd::setFont(const std::string& szFont)
	{
	
	}

	void CUIWnd::setText(const std::string& szContent)
	{

	}

	int CUIWnd::_pick(event& e)
	{
		int pickId = -1;
		if ((e.mouseX >= mPosition.x && e.mouseX <= mPosition.x + mSize.x) &&
			(e.mouseY >= mPosition.y && e.mouseY <= mPosition.y + mSize.y))
		{
			pickId = 0;
			for( auto item : mChildren )
			{
				if (-1 != item->_pick(e))
				{
					pickId = item->mId;
					break;
				}
			}
		}
		return pickId;
	}

}