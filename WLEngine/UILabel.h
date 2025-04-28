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
		struct iterator
		{
			std::string::iterator it;
			//using difference_type = std::ptrdiff_t;
			//using value_type = int;
			using pointer = char*;
			using reference = char&;

			iterator(std::string::iterator it) : it(it) {}

			reference operator*() const
			{
				return *it; 
			}

			pointer operator->() 
			{
				return &(*it); 
			}

			iterator& operator++() 
			{
				++it;
				return *this;
			}

			bool operator!=(const iterator& other) const 
			{
				return it != other.it;
			}
		};

		struct const_iterator
		{
			std::string::const_iterator cit;
			//using difference_type = std::ptrdiff_t;
			//using value_type = int;
			using pointer = const char*;
			using reference = const char&;

			const_iterator(std::string::const_iterator cit) : cit(cit) {}

			reference operator*() const 
			{
				return *cit; 
			}

			pointer operator->() 
			{
				return &(*cit); 
			}

			const_iterator& operator++() 
			{
				++cit;
				return *this;
			}

			bool operator!= (const const_iterator& other) const 
			{
				return cit != other.cit;
			}
		};

		CUILabel();
		virtual ~CUILabel();

		iterator begin() 
		{
			return iterator(mszContent.begin());
		}

		iterator end() 
		{
			return iterator(mszContent.end());
		}

		const_iterator cbegin() const 
		{
			return const_iterator(mszContent.cbegin());
		}

		const_iterator cend() const 
		{
			return const_iterator(mszContent.cend());
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
		std::vector<VertexVTC>	mDrawData;
	};

}