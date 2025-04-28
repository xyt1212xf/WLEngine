#pragma once
#include "ActorEntity.h"
#include "ScriptEntity.h"

namespace WL
{
	class CUIWnd;
	class CGuiEntity : public CScriptEntity
	{
		friend class CGuiRendererPass;
		friend class CGuiEngine;

	public:
		CGuiEntity();
		virtual ~CGuiEntity();
		virtual void _tick(UINT32 dt) final;
		bool initialise(const std::string& szFile);
		void setLuaEntityName(const std::string& szFun);

		void setPositon(float x, float y);
		void setX(float x);
		void setY(float y);
		
		float getX() const;
		float getY() const;
		
		void setWidth(float x);
		void setHeight(float x);

		void setPosition(const Vec3F& pos);
		void setPosition(UINT32 Id, const Vec3F& pos);

		void setSize(const Vec2F& szie);
		void setSize(UINT32 Id, const Vec2F& size);
		
		void setRotation(float fRadion);
		void setRotation(UINT32 Id, float fRadion);
		
		float getRotation() const;
		float getRotation(UINT32 Id) const;

		void setVisible(bool bValue);
		void setVisible(UINT32 Id, bool bValue);

		void setText(const std::string& szContent);
		void setText(UINT32 Id, const std::string& szContent);

		bool isVisible() const;
		bool isVisible(UINT32 Id) const;

		void addChildGui(const std::string& szFile);

		void loadGui(const std::string& szFile);
		void setAnimation(const std::string& szFile);
		void setAniLoop(bool bLoop);
		bool isAniLoop()const;

		bool setCurrentFrame(int nNumber);
		int getCurrentFrame() const;

		void setAniSpeed(float fSpeed);
		float getAniSpeed() const;

		bool isXYAxisSwap();
		void setXYAxisSwap(bool bValue);

		bool isXYAxisSwap(UINT32 Id);
		void setXYAxisSwap(UINT32 Id, bool bValue);

	private:
		bool mbLoop = true;
		CUIWnd* mpUIWnd = nullptr;
		std::string mLuaTableName = "";
		Vec2F	mPostion = {0, 0};
		Vec2F	mSize = {0, 0};
		DeclareScriptClass(CGuiEntity);
	};
}