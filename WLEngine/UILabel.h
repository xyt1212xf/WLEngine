#pragma once
#include "UIWnd.h"
namespace WL
{
	class CFont;	
	//Ö§³Ö·¶Î§for
	class CUILabel : public CUIWnd
	{
		friend class CGuiRendererPass;
	public:
		struct SIterator
		{
			std::string::iterator it;
			//using difference_type = std::ptrdiff_t;
			//using value_type = int;
			using pointer = char*;
			using reference = char&;

			SIterator(std::string::iterator it) : it(it) {}

			reference operator*() const
			{
				return *it; 
			}

			pointer operator->() 
			{
				return &(*it); 
			}

			SIterator& operator++() 
			{
				++it;
				return *this;
			}

			bool operator!=(const SIterator& other) const 
			{
				return it != other.it;
			}
		};

		struct SConst_iterator
		{
			std::string::const_iterator cit;
			//using difference_type = std::ptrdiff_t;
			//using value_type = int;
			using pointer = const char*;
			using reference = const char&;

			SConst_iterator(std::string::const_iterator cit) : cit(cit) {}

			reference operator*() const 
			{
				return *cit; 
			}

			pointer operator->() 
			{
				return &(*cit); 
			}

			SConst_iterator& operator++() 
			{
				++cit;
				return *this;
			}

			bool operator!= (const SConst_iterator& other) const 
			{
				return cit != other.cit;
			}
		};

		CUILabel();
		virtual ~CUILabel();

		SIterator begin() 
		{
			return SIterator(mszContent.begin());
		}

		SIterator end() 
		{
			return SIterator(mszContent.end());
		}

		SConst_iterator cbegin() const 
		{
			return SConst_iterator(mszContent.cbegin());
		}

		SConst_iterator cend() const 
		{
			return SConst_iterator(mszContent.cend());
		}
		const std::string& getText();
		void append(const std::string& szContent);
		virtual void setFont(const std::string& szFont) final;
		virtual void setText(const std::string& szContent)final;
		virtual void setPosition(const Vec3F& pos)final;

		void _updateDrawArea(Vec3F& pos);

		virtual void setSize(const Vec2F& size)final;
		void setFontSize(const Vec2F& size);
		Vec2F getFontSize() const;

	private:
#ifdef FONT_SDF	
		Vec2F mFontSize = { 8, 16 };
#else
		Vec2F mFontSize = { 0, 16 };
#endif
		std::string mszContent = "";
		CFont* mpFont = nullptr;
		std::vector<SVertexVTC>	mDrawData;
	};

}