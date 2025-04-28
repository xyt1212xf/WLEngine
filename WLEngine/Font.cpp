#include "WLEngine.h"
#include "Font.h"
#include "Texture.h"

namespace WL
{
	CFont::CFont()
	{

	}

	CFont::~CFont()
	{
		WL_DECREASE(mpTexture);
	}

	bool CFont::initialise()
	{
		std::string fileName = mResourceName + ".txt";
		std::ifstream fin;

		// Read in the font size and spacing between chars.
		fin.open(fileName);
		if (fin.fail())
		{
			return false;
		}
		int cellCount = 0;
		fin >> cellCount;
		mFontCells.resize(cellCount);

		char temp = 0;
		// Read in the 95 used ascii characters for text.
		for (auto& item : mFontCells)
		{
			fin.get(temp);
			while (temp != ' ')
			{
				fin.get(temp);
			}
			fin.get(temp);
			while (temp != ' ')
			{
				fin.get(temp);
			}

			fin >> item.left;
			fin >> item.right;
			fin >> item.size;
		}

		// Close the file.
		fin.close();
#ifdef FONT_SDF
		fileName = mResourceName + ".png";
#else
		fileName = mResourceName + ".dds";
#endif
		auto pTexture = static_cast<CTexture*>(GEngine->createResource(fileName, Texture));
		if (nullptr != pTexture)
		{
			pTexture->addRef();
			WL_DECREASE(mpTexture);
			mpTexture = pTexture;
			auto pMaterial = dynamic_cast<CMaterial*>(GEngine->findResource(CGuiRendererPass::szfontMaterName));
			if (nullptr != pMaterial)
			{
				pTexture->setSampleState(pMaterial->getSampler());
			}
		}
		return true;

	}

	const CFont::FontCell& CFont::getCell(char c) const
	{
		int nIndex = c - 32;
		if( nIndex < (int)mFontCells.size() )
		{
			return mFontCells[nIndex];
		}
		assert(false);
		static auto errorCell = FontCell();
		return errorCell;
	}

	const CTexture* CFont::getTexture() const
	{
		return mpTexture;
	}

}