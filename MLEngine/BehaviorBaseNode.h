#pragma once
#include "Common.h"
#include "Refcount.h"
#include "ScriptEntity.h"
namespace WL
{
	enum class NodeStatus : INT8
	{
		Success,
		Failure,
		Running,
	};
	class CBTNode : public CScriptEntity
	{
	public:
		CBTNode() = default;
		virtual ~CBTNode() = default;
		virtual NodeStatus execute() { return mStatus; };

	protected:
		NodeStatus mStatus = NodeStatus::Failure;

		DeclareScriptClass(CBTNode);
	};

	class CTaskNode : public CBTNode 
	{
	public:
		CTaskNode() = default;
		virtual ~CTaskNode() = default;
		NodeStatus execute();
		void registerLuaCallBack(const std::string& szFunction);

	private:
		std::string mLuaName = "";
		DeclareScriptClass(CTaskNode);
	};

	class CBaheviorTreeFunctionNode : public CBTNode
	{
	public:
		CBaheviorTreeFunctionNode() = default;
		virtual ~CBaheviorTreeFunctionNode();
		virtual NodeStatus execute();

		void addBTreeNode(CScriptEntity* pNode);
		void clearNode();

	protected:
		std::forward_list<CBTNode*>	mNodes;
		std::forward_list<CBTNode*>::iterator mIter = mNodes.before_begin();
	};
}