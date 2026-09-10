#pragma once
#include "Component.h"
#include "BehaviorNode.h"

namespace WL
{
	enum class BehaviorTreeStatus : INT8
	{
		Wait,
		Execute,
		Stop,
	};

	class CBehaviorTreeMgr;
	class CBehaviorTree : public CComponent, public CRefcount
	{
		friend class CBehaviorTreeMgr;
	public:
		CBehaviorTree();
		virtual ~CBehaviorTree();
		void start();
		void stop();
		void resume();
		void setRootNode(CBTNode* pNode);
		std::optional<CBTNode*> getBTreeNode(const std::string& sName) const;

	private:
		virtual void tick(UINT32 dt) final;

	private:
		static CBehaviorTree* sNewBehaviorTree;
		BehaviorTreeStatus mStatus = BehaviorTreeStatus::Wait;
		std::forward_list<CBTNode*>	mNodes;
		CBTNode* mpRootNode = nullptr;
		CBTNode* mpRunningNode = nullptr;
	};

	class CBehaviorTreeMgr
	{
	public:
		static void regBTreeToScrpit();
		static void unRegBTreeToScrpit();

		static  void createBT(const std::string& sName);
		static  bool setOperationBT(const std::string& sName);
		static std::optional<CBehaviorTree*> getBTree(const std::string& sName);
		static void createBTreeNode(const std::string& szNodeType, const std::string& szNodeName);
		static void setBTreeNodeParent(const std::string& szNode, const std::string& szParent);
		static bool setBTreeRootNode(const std::string& szNode, CScriptEntity*);
		
	private:
		static std::forward_list<CBehaviorTree*> gBTrees;
		static CBehaviorTree* sNewBehaviorTree;
	};
}
