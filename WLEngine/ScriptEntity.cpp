#include <functional>
#include "LuaBind.h"
#include "ScriptEntity.h"
#include "ScriptEngine.h"
#include "WLEngine.h"
#include "Scene.h"

namespace WL
{
	bool CScriptEntity::bOverRegister = true;
	std::list<CScriptEntity*>* CScriptEntity::spRegisterClassObject = nullptr;
	CScriptEntity::CScriptEntity()
	{
		if (bOverRegister)
		{
			if (nullptr == spRegisterClassObject)
			{
				spRegisterClassObject = new std::list<CScriptEntity*>;
			}
			spRegisterClassObject->emplace_back(this);
		}
		static UINT32 handle = 0;
		mHandle = ++handle;
	}

	CScriptEntity::~CScriptEntity()
	{
		if (nullptr != spRegisterClassObject)
		{
			delete spRegisterClassObject;
			spRegisterClassObject = nullptr;
		}
	}
	void CScriptEntity::registerClass()
	{

	}

	void CScriptEntity::registerMember()
	{

	}


	UINT32 CScriptEntity::getRegisterHandle()
	{
		return mHandle;
	}	

	void CScriptEntity::registerHandle()
	{

	}

	void CScriptEntity::enterScene()
	{
		auto pScene = GEngine->getNextScenePtr();
		pScene = pScene == nullptr ? GEngine->getCurrentScenePtr() : pScene;
		pScene->addEntity(this);
	}

	void CScriptEntity::leaveScene()
	{
		auto pScene = GEngine->getCurrentScenePtr();
		if (nullptr != pScene)
		{
			pScene->removeEntity(this);
		}
	}
}