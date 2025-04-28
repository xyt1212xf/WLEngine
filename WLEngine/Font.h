#pragma once
#include "Resource.h"
namespace WL
{
	class CFont : public CResource
	{
		friend class CFontEngine;
		struct FontCell
		{
			float left, right;
			int size;
		};
	public:
		CFont();
		virtual ~CFont();
		virtual bool initialise() final; 
		const FontCell& getCell(char c) const;
		const CTexture* getTexture() const;

	private:
		std::vector<FontCell>	mFontCells;
		CTexture*	mpTexture = nullptr;
	};
}