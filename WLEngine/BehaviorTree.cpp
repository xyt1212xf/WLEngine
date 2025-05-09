#include "BehaviorTree.h"
#include "WLEngine.h"
namespace WL
{
	CBehaviorTree* CBehaviorTree::sNewBehaviorTree = nullptr;
	CBehaviorTree::CBehaviorTree()
	{
		mComponentName = "BehaviorTree";
	}

	CBehaviorTree::~CBehaviorTree()
	{
		WL_DECREASE(mpRootNode, BehaviorTree);
	}

	void CBehaviorTree::start()
	{
		if (mpRootNode)
		{
		}
	}

	void CBehaviorTree::stop()
	{

	}

	void CBehaviorTree::resume()
	{

	}

	void CBehaviorTree::setRootNode(CBTNode* pNode)
	{
		pNode->addRef();
		WL_DECREASE(mpRootNode);
		mpRootNode = pNode;
		mpRunningNode = mpRootNode;
	}

	std::optional<CBTNode*> CBehaviorTree::getBTreeNode(const std::string& sName) const
	{
		for (auto item : mNodes)
		{
			if (item->getName() == sName)
			{
				return item;
			}
		}
		return std::nullopt;
	}

	void CBehaviorTree::tick(UINT32 dt)
	{
		if (nullptr != mpRunningNode)
		{
			mpRunningNode->execute();
		}
	}

	//////////////////////////////////////////////////////////////////////////	
	std::forward_list<CBehaviorTree*> CBehaviorTreeMgr::gBTrees;
	CBehaviorTree* CBehaviorTreeMgr::sNewBehaviorTree = nullptr;

	void CBehaviorTreeMgr::regBTreeToScrpit()
	{
		auto pLua = GEngine->getLuaState();
		CRegisterFun<void>::registrFun<void(*)(), &CBehaviorTreeMgr::regBTreeToScrpit>(pLua, "initBT");
		CRegisterFun<void>::registrFun<void(*)(const std::string&), &CBehaviorTreeMgr::createBT, const std::string&>(pLua, "createBT");
		CRegisterFun<bool>::registrFun<bool(*)(const std::string&), &CBehaviorTreeMgr::setOperationBT, const std::string&>(pLua, "setOperationBT");
		CRegisterFun<void>::registrFun<void(*)(const std::string&, const std::string&), &CBehaviorTreeMgr::createBTreeNode, const std::string&, const std::string&>(pLua, "createBTreeNode");
		CRegisterFun<void>::registrFun<void(*)(const std::string&, const std::string&), &CBehaviorTreeMgr::setBTreeNodeParent, const std::string&, const std::string&>(pLua, "setBTreeNodeParent");
		CRegisterFun<bool>::registrFun<bool(*)(const std::string&, CScriptEntity*), &CBehaviorTreeMgr::setBTreeRootNode, const std::string&, CScriptEntity*>(pLua, "setBTreeRootNode");
	}

	void CBehaviorTreeMgr::unRegBTreeToScrpit()
	{
		for (auto item : CBehaviorTreeMgr::gBTrees)
		{
			WL_DECREASE(item);
		}
		gBTrees.clear();
	}
	

	void CBehaviorTreeMgr::createBT(const std::string& sName)
	{
		auto pBTree = WL_NEW(CBehaviorTree, BehaviorTree);
		pBTree->setComponentName(sName);
		CBehaviorTreeMgr::gBTrees.emplace_front(pBTree);
		pBTree->addRef();
		setOperationBT(sName);
	}


	bool CBehaviorTreeMgr::setOperationBT(const std::string& sName)
	{
		for (auto item : CBehaviorTreeMgr::gBTrees)
		{
			if (item->getComponentName() == sName)
			{
				sNewBehaviorTree = item;
				return true;
			}
		}
		return false;
	}

	std::optional<CBehaviorTree*> CBehaviorTreeMgr::getBTree(const std::string& sName)
	{
		for (auto item : CBehaviorTreeMgr::gBTrees)
		{
			if (item->getComponentName() == sName)
			{
				return item;
			}
		}
		return std::nullopt;
	}

	void CBehaviorTreeMgr::createBTreeNode(const std::string& szNodeType, const std::string& szNodeName)
	{
		if (sNewBehaviorTree)
		{
			CBTNode* pNode = nullptr;
			if ("Sequence" == szNodeType)
			{
				pNode = WL_NEW(CSequenceNode, BehaviorTree);
			}
			else if ("Selector" == szNodeType)
			{
				pNode = WL_NEW(CSelectorNode, BehaviorTree);
			}
			else if ("Parallel" == szNodeType)
			{
				pNode = WL_NEW(CParallelNode, BehaviorTree);
			}
			else if ("Decorator" == szNodeType)
			{
				pNode = WL_NEW(CDecoratorNode, BehaviorTree);
			}
			else if ("Repeat" == szNodeType)
			{
				pNode = WL_NEW(CRepeatNode, BehaviorTree);
			}
			else if ("Task" == szNodeType)
			{
				pNode = WL_NEW(CTaskNode, BehaviorTree);
			}
			if (nullptr != pNode)
			{
				pNode->setName(szNodeName);
				sNewBehaviorTree->mNodes.emplace_front(pNode);
			}
		}
	}

	void CBehaviorTreeMgr::setBTreeNodeParent(const std::string& szNode, const std::string& szParent)
	{
		if (sNewBehaviorTree)
		{
			auto pNode = sNewBehaviorTree->getBTreeNode(szNode);
			auto pParent = sNewBehaviorTree->getBTreeNode(szParent);
			if (pNode.has_value() && pParent.has_value())
			{
				auto pFunctionNode = dynamic_cast<CBaheviorTreeFunctionNode*>(pParent.value());
				
				if (nullptr != pFunctionNode)
				{
					pFunctionNode->addBTreeNode(pNode.value());
				}
			}
		}
	}


	bool CBehaviorTreeMgr::setBTreeRootNode(const std::string& szBTree, CScriptEntity* pRootNode)
	{
		auto pBTree = CBehaviorTreeMgr::getBTree(szBTree);
		auto* pNode = dynamic_cast<CBaheviorTreeFunctionNode*>(pRootNode);
		if (pNode)
		{
			pBTree.value()->setRootNode(pNode);
			return true;
		}	
		return false;
	}

}