#pragma once
#include "Resource.h"
#include "TextureBase.h"
#include "Message.h"
#include "UIHeader.h"
/*gui三角形的点顺序，顺时针填充
0   1
----|
| / |
|/--|
2   3
*/

namespace WL
{	
	class CGuiEntity;
	class CTexture;
	class CUIWnd : public CResource
	{
		friend class CGuiRendererPass;
		friend class CGuiEngine;
		friend class CGuiEntity;
		friend class CResourceMgr;

	public:
		CUIWnd();
		virtual ~CUIWnd();
		virtual void setTexture(const std::string& szFile);
		virtual void setAniLoop(bool bLoop){}
		virtual bool isAniLoop()const { return false; }
		virtual bool setCurrentFrame(int nNumber) { return false; }
		virtual int getCurrentFrame() const { return 0; }
		virtual void setAniSpeed(float fSpeed) {}
		virtual float getAniSpeed() const { return 0; }

		virtual void _tick(UINT32 dt);
		WndType getWndType() const;
		
		virtual void setPosition(const Vec3F& pos);
		const Vec3F& getPosition() const;
		virtual void setSize(const Vec2F& size);
		
		const Vec2F& getSize() const;

		
		void addChild(CUIWnd* pChild);
		void removeChild(CUIWnd* pChild);
		void setRotation(float fRadian);
		float getRotation()const;
		void setId(UINT32 Id);
		UINT32 getId();
		std::optional<CUIWnd*> getchild(UINT32 id)const;
		void setVisible(bool bValue);
		bool isVisible() const;
		bool isXYAxisSwap();
		void setXYAxisSwap(bool bValue);
		virtual void setFont(const std::string& szFont);
		virtual void setText(const std::string& szContent);

	private:
		int _pick(SEvent& e);
		void _updateArea();
	protected:
		WndType	mWndType = ui_wnd;
		
		union Data 
		{
			struct
			{
				SVertexVTC	mTop;
				SVertexVTC	mLeft;
				SVertexVTC	mRight;
				SVertexVTC	mBottom;
			}vertices;
			SVertexVTC	mArea[4];
			Data()
			{
				new(&vertices) decltype(vertices);
			}
			~Data()
			{
				vertices.~decltype(vertices)();
			}
		};
		Data mData;
		
		Vec3F mPosition = { 0,0,0 };
		Vec3F mDrawPosition = { 0,0,0 };
		Vec2F mSize = {1,1};
		UINT32 mId = 0;
		CUIWnd* mpParentWnd = nullptr;
		bool mXYAxisSwap = false;
		bool mVisible = true;
		float mRadian = 0.0f;
		std::string mTextureName = "";
		std::map<TextureUseType, CTexture*>	mTextureMap;
		std::list<CUIWnd*>	mChildren;
	};
}