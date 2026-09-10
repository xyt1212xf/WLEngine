#include "UILabel.h"
#include "WLEngine.h"
#include <iostream>
namespace WL
{
	CUILabel::CUILabel()
	{
		mWndType = ui_label;
	}

	CUILabel::~CUILabel()
	{

	}

	void CUILabel::setText(const std::string& szContent)
	{
		auto nSize = szContent.length();
		mszContent = szContent;
		mDrawData.resize(nSize * 4);
		int nStep = 0;
		float nOffset = 0.0f;
#ifdef FONT_SDF	
		static int i = 1;
		mFontSize.x *= i;
		mFontSize.y *= i;
		++i;
#endif
		for( auto c : szContent )
		{
			const auto& cell = mpFont->getCell(c);
			nOffset = static_cast<float>(cell.size);

			//position;
			mDrawData[0 + nStep].position = { 0, 0, 0 };
			mDrawData[1 + nStep].position = { nOffset + mFontSize.x, 0, 0 };
			mDrawData[2 + nStep].position = { 0, -mFontSize.y, 0 };
			mDrawData[3 + nStep].position = { nOffset + mFontSize.x, -mFontSize.y, 0 };

			//uv
#ifdef FONT_SDF	
			mDrawData[0 + nStep].uv = { 0, 0.0f };
			mDrawData[1 + nStep].uv = { 1, 0.0f };
			mDrawData[2 + nStep].uv = { 0, 1.0f };
			mDrawData[3 + nStep].uv = { 1, 1.0f };
#else
			mDrawData[0 + nStep].uv = { cell.left, 0.0f};
			mDrawData[1 + nStep].uv = { cell.right, 0.0f};
			mDrawData[2 + nStep].uv = { cell.left, 1.0f};
			mDrawData[3 + nStep].uv = { cell.right, 1.0f};
#endif
			//color
			mDrawData[0 + nStep].color = { 1,1,1,1 };
			mDrawData[1 + nStep].color = { 1,1,1,1 };
			mDrawData[2 + nStep].color = { 1,1,1,1 };
			mDrawData[3 + nStep].color = { 1,1,1,1 };

			nStep += 4;
		}
	}

	void CUILabel::setPosition(const Vec3F& _pos)
	{
		Vec3F pos = _pos;
		auto offset = pos - mPosition;

		mPosition += offset;
		pos = { pos.x - GEngine->getWindowConfigPtr()->nWidth * 0.5f,
				GEngine->getWindowConfigPtr()->nHeight * 0.5f - pos.y,
				pos.z };

		_updateDrawArea(pos);

	}


	void CUILabel::_updateDrawArea(Vec3F& pos)
	{
		for (size_t i = 0; i < mDrawData.size(); i += 4)
		{
			mDrawData[i + 0].position = pos;

			mDrawData[i + 1].position.x += pos.x;
			mDrawData[i + 1].position.y += pos.y;

			mDrawData[i + 2].position.x += pos.x;
			mDrawData[i + 2].position.y += pos.y;

			mDrawData[i + 3].position.x += pos.x;
			mDrawData[i + 3].position.y += pos.y;
			float offset = (mDrawData[i + 1].position.x - mDrawData[i + 0].position.x);
			pos.x += offset + 1;
		}
	}

	void CUILabel::setSize(const Vec2F& size)
	{
		mSize = size;
	}

	void CUILabel::setFontSize(const Vec2F& size)
	{
		mFontSize = size;
	}

	Vec2F CUILabel::getFontSize() const
	{
		return mFontSize;
	}

	const std::string& CUILabel::getText()
	{
		return mszContent;
	}

	void CUILabel::append(const std::string& szContent)
	{
		mszContent.append(szContent);
	}

	void CUILabel::setFont(const std::string& szFont)
	{
		assert(szFont.length() > 0);
		auto pResouce = GEngine->findResource(szFont);
		if (nullptr != pResouce)
		{
			pResouce->addRef();
			WL_DECREASE(mpFont);
			mpFont = dynamic_cast<CFont*>(pResouce);
			assert(mpFont);
		}
		else
		{
			mpFont = static_cast<CFont*>(GEngine->createResource(szFont, ResourceType::Font));
		}

		mTextureMap[TextureUseType::Base] = const_cast<CTexture*>(mpFont->getTexture());
		mTextureMap[TextureUseType::Base]->addRef();
	}

}