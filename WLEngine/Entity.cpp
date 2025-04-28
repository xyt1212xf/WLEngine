#include "Entity.h"
#include "Scene.h"
namespace WL
{
	static UINT32 id;
	CEntity::CEntity()
	{		
		mID = ++id;
	}

	CEntity::CEntity(const CEntity& entity)
	{
		mID = ++id;
		mpScene = entity.mpScene;		
		mszName = entity.mszName;
		mbVisible = entity.mbVisible;
	}

	CEntity::CEntity(CEntity&& entity) noexcept
	{
		CEntity::operator=(entity);
	}

	CEntity::~CEntity()
	{

	}

	EntityType CEntity::getEntityType() const
	{
		return mType;
	}

	const std::string& CEntity::getName() const
	{
		return mszName;
	}

	void CEntity::setName(const std::string& szName)
	{
		mszName = szName;
	}

	UINT32 CEntity::getID() const
	{
		return mID;
	}

	void CEntity::setVisible(bool bValue)
	{
		mbVisible = bValue;
		if (nullptr != mpScene)
		{

		}
	}

	bool CEntity::isVisible() const
	{
		return mbVisible;
	}

	void CEntity::show()
	{
		setVisible(true);
	}

	void CEntity::hide()
	{
		setVisible(false);
	}

	bool CEntity::enterScene(CScene* pScene)
	{
		return pScene->addEntity(this);
	}

	bool CEntity::leaveScene(CScene* pScene)
	{
		return pScene->removeEntity(this);
	}

	CEntity& CEntity::operator=(const CEntity& entity)
	{
		mpScene = entity.mpScene;
		mID = entity.mID;
		mbVisible = entity.mbVisible;
		mszName = entity.mszName;
		return *this;
	}

	CEntity& CEntity::operator=(CEntity&& entity) noexcept
	{
		mpScene = entity.mpScene;
		mID = entity.mID;
		mbVisible = entity.mbVisible;
		mszName = entity.mszName;
		entity.mpScene = nullptr;
		return *this;
	}

	void CEntity::_tick(UINT32 dt)
	{

	}

}