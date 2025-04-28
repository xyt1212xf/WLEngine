#include "BehaviorNode.h"
#include "WLEngine.h"
namespace WL
{

	//////////////////////////////////////////////////////////////////////////
	DefineScriptClass(CConditionNode)
	CConditionNode::CConditionNode()
	{

	}

	CConditionNode::~CConditionNode()
	{

	}

	NodeStatus CConditionNode::execute()
	{
		return NodeStatus::Failure;
	}

	void CConditionNode::registerClass()
	{
		CRegisterClass<CConditionNode>::registerClass(GEngine->getLuaState());
	}

	void CConditionNode::registerMember()
	{
	}

	//////////////////////////////////////////////////////////////////////////
	DefineScriptClass(CSequenceNode)
	CSequenceNode::CSequenceNode()
	{

	}

	CSequenceNode::~CSequenceNode()
	{

	}

	void CSequenceNode::registerClass()
	{
		CRegisterClass<CSequenceNode>::registerClass(GEngine->getLuaState());
	}

	void CSequenceNode::registerMember()
	{
		auto pLua = GEngine->getLuaState();
		CRegisterFun<void>::registerClassMemberFun<CSequenceNode, void(CBaheviorTreeFunctionNode::*)(CScriptEntity*), &CBaheviorTreeFunctionNode::addBTreeNode, CScriptEntity*>(pLua, "addNode");
	}


	NodeStatus CSequenceNode::execute()
	{
		for (auto item : mNodes)
		{
			auto status = item->execute();
			if (NodeStatus::Failure == status)
			{
				return status;
			}
		}
		return NodeStatus::Success;
	}

	//////////////////////////////////////////////////////////////////////////
	DefineScriptClass(CSelectorNode)
	CSelectorNode::CSelectorNode()
	{

	}

	CSelectorNode::~CSelectorNode()
	{

	}

	void CSelectorNode::registerClass()
	{
		CRegisterClass<CSelectorNode>::registerClass(GEngine->getLuaState());
	}

	void CSelectorNode::registerMember()
	{
		auto pLua = GEngine->getLuaState();
		CRegisterFun<void>::registerClassMemberFun<CSelectorNode, void(CBaheviorTreeFunctionNode::*)(CScriptEntity*), &CBaheviorTreeFunctionNode::addBTreeNode, CScriptEntity*>(pLua, "addNode");
	}

	NodeStatus CSelectorNode::execute()
	{
		for (auto item : mNodes)
		{
			auto status = item->execute();
			if (NodeStatus::Failure != status )
			{
				return status;
			}
		}
		return NodeStatus::Failure;
	}

	//////////////////////////////////////////////////////////////////////////
	DefineScriptClass(CParallelNode)
	CParallelNode::CParallelNode()
	{

	}

	CParallelNode::~CParallelNode()
	{

	}

	void CParallelNode::registerClass()
	{
		CRegisterClass<CParallelNode>::registerClass(GEngine->getLuaState());
	}

	void CParallelNode::registerMember()
	{
		auto pLua = GEngine->getLuaState();
		CRegisterFun<void>::registerClassMemberFun<CParallelNode, void(CBaheviorTreeFunctionNode::*)(CScriptEntity*), &CBaheviorTreeFunctionNode::addBTreeNode, CScriptEntity*>(pLua, "addNode");
	}

	NodeStatus CParallelNode::execute()
	{
		return NodeStatus::Failure;
	}

	//////////////////////////////////////////////////////////////////////////
	DefineScriptClass(CDecoratorNode)
	CDecoratorNode::CDecoratorNode()
	{

	}

	CDecoratorNode::~CDecoratorNode()
	{

	}
	void CDecoratorNode::registerClass()
	{
		CRegisterClass<CDecoratorNode>::registerClass(GEngine->getLuaState());
	}

	void CDecoratorNode::registerMember()
	{
		auto pLua = GEngine->getLuaState();
		CRegisterFun<void>::registerClassMemberFun<CDecoratorNode, void(CBaheviorTreeFunctionNode::*)(CScriptEntity*), &CBaheviorTreeFunctionNode::addBTreeNode, CScriptEntity*>(pLua, "addNode");
	}

	NodeStatus CDecoratorNode::execute()
	{
		return NodeStatus::Failure;
	}

	//////////////////////////////////////////////////////////////////////////
	DefineScriptClass(CRepeatNode)
	CRepeatNode::CRepeatNode()
	{

	}

	CRepeatNode::~CRepeatNode()
	{

	}

	void CRepeatNode::registerClass()
	{
		CRegisterClass<CRepeatNode>::registerClass(GEngine->getLuaState());
	}

	void CRepeatNode::registerMember()
	{
		auto pLua = GEngine->getLuaState();
		CRegisterFun<void>::registerClassMemberFun<CRepeatNode, void(CBaheviorTreeFunctionNode::*)(CScriptEntity*), &CBaheviorTreeFunctionNode::addBTreeNode, CScriptEntity*>(pLua, "addNode");
	}

	NodeStatus CRepeatNode::execute()
	{
		return NodeStatus::Failure;
	}

}