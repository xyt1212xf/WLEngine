#include "BehaviorBaseNode.h"
#include "WLEngine.h"
#include "ScriptEntity.h"
namespace WL
{
	DefineScriptClass(CBTNode)
	DefineScriptClass(CTaskNode)

	void CBTNode::registerClass()
	{
		CRegisterClass<CBTNode>::registerClass(GEngine->getLuaState());
	}

	void CBTNode::registerMember()
	{
	}

	void CTaskNode::registerClass()
	{
		CRegisterClass<CTaskNode>::registerClass(GEngine->getLuaState());
	}

	void CTaskNode::registerMember()
	{
		auto pLua = GEngine->getLuaState();
		CRegisterFun<void>::registerClassMemberFun<CTaskNode, void(CTaskNode::*)(const std::string&), &CTaskNode::registerLuaCallBack, const std::string&>(pLua, "callBack");
	}

	NodeStatus CTaskNode::execute()
	{
		if (mLuaName.length() > 0)
		{
			lua_State* pLua = GEngine->getLuaState();
			int nTop = lua_gettop(pLua);

			lua_getglobal(pLua, mLuaName.c_str());
			if (lua_istable(pLua, -1))
			{
				lua_getfield(pLua, -1, "Execute");
				if (lua_isfunction(GEngine->getLuaState(), -1))
				{
					//lua_pushvalue(pLua, -2);
					if (lua_pcall(pLua, 0, 1, 0) == LUA_OK)
					{
						bool bResult = static_cast<bool>(lua_toboolean(pLua, -1));
						mStatus = bResult ? NodeStatus::Success : NodeStatus::Failure;
					}
				}
			}
			lua_settop(pLua, nTop);
		}
		return mStatus;
	}

	void CTaskNode::registerLuaCallBack(const std::string& szFunction)
	{
		mLuaName = szFunction;
	}

	void CBaheviorTreeFunctionNode::addBTreeNode(CScriptEntity* pNode)
	{
		auto pBTreeNode = dynamic_cast<CBTNode*>(pNode);
		if (pBTreeNode)
		{
			mIter = mNodes.insert_after(mIter, pBTreeNode);
			pNode->addRef();
		}
	}


	void CBaheviorTreeFunctionNode::clearNode()
	{
		for (auto item : mNodes)
		{
			WL_DECREASE(item);
		}
		mNodes.clear();
	}

	CBaheviorTreeFunctionNode::~CBaheviorTreeFunctionNode()
	{
		clearNode();
	}

	NodeStatus CBaheviorTreeFunctionNode::execute()
	{
		return NodeStatus::Failure;
	}

}