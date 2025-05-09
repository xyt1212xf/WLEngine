#pragma once
#include "Refcount.h"
#include "Component.h"
namespace WL
{
	enum EntityType
	{
		Entity = 1 << 1,
		Camera = 1 << 2,
		Light = 1 << 3,
		Actor = 1 << 4,
		Sky = 1 << 5,
		Terrain = 1 << 6,
		SkyBox = 1 << 7,
		Ocean = 1 << 8,
		Gui = 1 << 9,
		AllEntity = 0xFFFFFFFF
	};
	class CScene;
	class CEntityFactory;
	class WLENGINE_API CEntity : public ComponentGrounp, public CRefcount
	{
		friend CScene;
		friend CEntityFactory;

	public:
		CEntity();
		CEntity(const CEntity& entity);
		CEntity(CEntity&& entity)noexcept;
		virtual ~CEntity();
		EntityType getEntityType() const;
		const std::string& getName() const;
		void setName(const std::string& szName);
		UINT32 getID() const;
		void setVisible( bool bValue);
		bool isVisible() const;
		void show();
		void hide();
		bool enterScene(CScene* pScene);
		bool leaveScene(CScene* pScene);
		CEntity& operator = (CEntity&& entity)noexcept;
		CEntity& operator = (const CEntity& entity);

	protected:
		virtual void _tick(UINT32 dt);

	protected:
		std::string mszName = "Entity";
		EntityType	mType = Entity;
		UINT32		mID = 0;
		CScene*		mpScene = nullptr;
		bool		mbVisible = true;
	};


}