#include "ActorEntity.h"
#include "Renderer.h"
#include "WLEngine.h"
#include "FrameAniComponet.h"
#include "BehaviorTree.h"
#include "ComponentFactory.h"
namespace WL
{
	DefineScriptClass(CActorEntity)
	CActorEntity::CActorEntity()
	{
		mType = Actor;
	}

	CActorEntity::~CActorEntity()
	{
		for (auto item : mModelInstanceMap)
		{
			//SafeDelete(item.second);
			WL_DELETE(item.second, Instance);
		}
		mModelInstanceMap.clear();
	}

	void CActorEntity::registerClass()
	{
		CRegisterClass<CActorEntity>::registerClass(GEngine->getLuaState());
	}


	void CActorEntity::addBehaviorTree(const std::string& szComponent)
	{
		auto pBTree = CBehaviorTreeMgr::getBTree(szComponent);
		if (pBTree.has_value())
		{
			addComponent(pBTree.value());
		}
	}

	void CActorEntity::registerMember()
	{
		auto pLua = GEngine->getLuaState();
		CRegisterFun<void>::registerClassMemberFun<CActorEntity, void(CActorEntity::*)(const std::string&), &CActorEntity::addBehaviorTree, const std::string&>(pLua, "addBehaviorTree");
		CRegisterFun<void>::registerClassMemberFun<CActorEntity, void(CEntity::*)(const std::string&), &CEntity::setEntityName, const std::string&>(pLua, "setName");
		CRegisterFun<const std::string&>::registerClassMemberFun<CActorEntity, const std::string& (CEntity::*)()const, &CEntity::getEntityName>(pLua, "getName");

		CRegisterFun<void>::registerClassMemberFun<CActorEntity, void(CActorEntity::*)(const std::string&), &CActorEntity::addComponentByScript, const std::string&>(pLua, "addComponent");
		CRegisterFun<void>::registerClassMemberFun<CActorEntity, void(CActorEntity::*)(const std::string&), &CActorEntity::removeComponentByScript, const std::string&>(pLua, "removeComponent");
		CRegisterFun<bool>::registerClassMemberFun<CActorEntity, bool(CActorEntity::*)(const std::string&), &CActorEntity::hasComponetByScript, const std::string&>(pLua, "hasComponet");

		CRegisterFun<void>::registerClassMemberFun<CActorEntity, void(CScriptEntity::*)(), &CScriptEntity::enterScene>(pLua, "enterScene");
		CRegisterFun<void>::registerClassMemberFun<CActorEntity, void(CScriptEntity::*)(), &CScriptEntity::leaveScene>(pLua, "leaveScene");
		
		CRegisterFun<void>::registerClassMemberFun<CActorEntity, void(CActorEntity::*)(CScriptEntity*), &CActorEntity::test, CScriptEntity*>(pLua, "test");
	}

	void CActorEntity::addModel(CModel* pModel, INT32 nSlot)
	{
		pModel->addRef();
		CModelInstance* pInstance = WL_NEW(CModelInstance, Instance)(pModel);
		auto [iter, insert] = mModelInstanceMap.try_emplace(nSlot, pInstance);
		if (!insert)
		{
			WL_DELETE(iter->second, Instance);
			iter->second = pInstance;
		}
		mAABB.unionBoundBox(pInstance->getBoundBox());
		WL_DECREASE(pModel);
	}

	void CActorEntity::addModel(const std::string& szModel, INT32 nSlot)
	{
		auto pModel = dynamic_cast<CModel*>(GEngine->createResource(szModel, Model));
		addModel(pModel, 0);
		WL_DECREASE(pModel);
	}


	void CActorEntity::addModel(CModel* pModel)
	{
		int nSlot = mModelInstanceMap.size();
		while (true)
		{
			auto iter = mModelInstanceMap.find(nSlot);
			if (iter != mModelInstanceMap.end())
			{
				++nSlot;
				continue;
			}
			else
			{
				addModel(pModel, nSlot);
				break;
			}
		}
	}


	void CActorEntity::addModel(CModelInstance* pModelInstance)
	{
		if (nullptr != pModelInstance)
		{
			int nSlot = mModelInstanceMap.size();
			while (true)
			{
				auto iter = mModelInstanceMap.find(nSlot);
				if (iter != mModelInstanceMap.end())
				{
					++nSlot;
					continue;
				}
				else
				{
					mModelInstanceMap[nSlot] = pModelInstance;
					pModelInstance->addRef();
					break;
				}
			}
		}
	}

	void CActorEntity::removeModel(CModel* pModel)
	{
		auto iter = mModelInstanceMap.begin();
		auto iterEnd = mModelInstanceMap.end();
		while (iter != iterEnd)
		{
			if (iter->second->getModel() == pModel)
			{
				mModelInstanceMap.erase(iter);
				//SafeDelete(iter->second);
				WL_DELETE(iter->second, Instance);
				break;
			}
		}
		if (mModelInstanceMap.size() == 0)
		{
			removeComponent<CCollision>();
		}
	}

	void CActorEntity::removeModel(INT32 nPart)
	{
		auto iter = mModelInstanceMap.find(nPart);
		if (iter != mModelInstanceMap.end())
		{
			//SafeDelete(iter->second);
			WL_DELETE(iter->second, Instance);
			mModelInstanceMap.erase(iter);
		}
		if (mModelInstanceMap.size() == 0)
		{
			removeComponent<CCollision>();
		}
	}

	void CActorEntity::draw(UINT32 nTime)
	{
	}

	void CActorEntity::addComponentByScript(const std::string& componentName)
	{
		if (!hasComponetByScript(componentName))
		{
			if (auto pComponent = CComponentFactory::getSinglePtr()->CreateComponent(componentName))
			{

			}

		}
	}

	void CActorEntity::removeComponentByScript(const std::string& componentName)
	{
		auto pComponent = getComponetByName(componentName);
		if(nullptr != pComponent)
		{
			removeComponent(pComponent);
		}
	}

	bool CActorEntity::hasComponetByScript(const std::string& componentName)
	{
		return getComponetByName(componentName) != nullptr ? true : false;
	}

	void CActorEntity::test(CScriptEntity* p)
	{
		auto xx = dynamic_cast<CActorEntity*>(p);
		int a = 0;
	}

	void CActorEntity::setPosition(const Vec3F& pos)
	{
		CTranformComponet::setPosition(pos);
		CTranformComponet::updateMatrix();
		for (auto item : mModelInstanceMap)
		{
			(item.second)->setPosition(pos);
		}
	}

	void CActorEntity::setScale(const Vec3F& scale)
	{
		CTranformComponet::setScale(scale);
		CTranformComponet::updateMatrix();
		for (auto item : mModelInstanceMap)
		{
			(item.second)->setScale(scale);
		}
	}


	void CActorEntity::setScale(float fScale)
	{
		setScale(Vec3F(fScale, fScale, fScale));
	}

	void CActorEntity::setRotation(const Vec3F& rotation)
	{
		CTranformComponet::setRotation(rotation);
		CTranformComponet::updateMatrix();
		for (auto item : mModelInstanceMap)
		{
			(item.second)->setRotation((item.second)->getRotation() + rotation);
		}
	}


	void CActorEntity::setViewMT(Matrix44* pMT)
	{
		for (auto item : mModelInstanceMap)
		{
			for (auto child : (item.second)->getAllMeshInfo())
			{
				child.pMeshInstance->setViewMT(pMT);
			}
		}
	}

	void CActorEntity::setTransposeViewMT(Matrix44* pMT)
	{
		for (auto item : mModelInstanceMap)
		{
			for (auto child : (item.second)->getAllMeshInfo())
			{
				child.pMeshInstance->setTransposeViewMT(pMT);
			}
		}
	}

	void CActorEntity::setProjectMT(Matrix44* pMT)
	{
		for (auto item : mModelInstanceMap)
		{
			for (auto child : (item.second)->getAllMeshInfo())
			{
				child.pMeshInstance->setProjectMT(pMT);
			}
		}
	}

	void CActorEntity::setTransposeProjectMT(Matrix44* pMT)
	{
		for (auto item : mModelInstanceMap)
		{
			for (auto child : (item.second)->getAllMeshInfo())
			{
				child.pMeshInstance->setTransposeProjectMT(pMT);
			}
		}
	}

	const std::map<INT32, CModelInstance*>& CActorEntity::getModels() const
	{
		return mModelInstanceMap;
	}


	CModelInstance* CActorEntity::getModelInstance(int nKey /*= 0*/) const
	{
		CModelInstance* pInstance = nullptr;
		auto iter = mModelInstanceMap.find(nKey);
		if (iter != mModelInstanceMap.end())
		{
			pInstance = iter->second;
		}
		return pInstance;
	}

	void CActorEntity::_tick(UINT32 dt)
	{
		for (auto [k, value] : mComponent)
		{
			value->tick(dt);
		}
	}

}