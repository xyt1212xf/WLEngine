#pragma once
#include "Resource.h"
namespace WL
{
	class CFont : public CResource
	{
		friend class CFontEngine;
		struct SFontCell
		{
			float left, right;
			int size;
		};
	public:
		CFont();
		virtual ~CFont();
		virtual bool initialise() final; 
		const SFontCell& getCell(char c) const;
		const CTexture* getTexture() const;

	private:
		std::vector<SFontCell>	mFontCells;
		CTexture*	mpTexture = nullptr;
	};
}