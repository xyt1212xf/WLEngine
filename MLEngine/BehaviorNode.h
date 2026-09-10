#pragma once

#include "BehaviorBaseNode.h"

namespace WL
{
	
	//////////////////////////////////////////////////////////////////////////
	class CConditionNode : public CBaheviorTreeFunctionNode
	{
	public:
		CConditionNode();
		virtual ~CConditionNode();
		virtual NodeStatus execute() final;

		DeclareScriptClass(CConditionNode);
	};

	//////////////////////////////////////////////////////////////////////////
	class CSequenceNode : public CBaheviorTreeFunctionNode
	{
	public:
		CSequenceNode();
		virtual ~CSequenceNode();
		virtual NodeStatus execute() final;

		DeclareScriptClass(CSequenceNode);
	};


	//////////////////////////////////////////////////////////////////////////
	class CSelectorNode : public CBaheviorTreeFunctionNode
	{
	public:
		CSelectorNode();
		virtual ~CSelectorNode();
		virtual NodeStatus execute() final;

		DeclareScriptClass(CSelectorNode);
	};

	//////////////////////////////////////////////////////////////////////////
	class CParallelNode : public CBaheviorTreeFunctionNode
	{
	public:
		CParallelNode();
		virtual ~CParallelNode();
		virtual NodeStatus execute() final;
		DeclareScriptClass(CParallelNode);
	};


	//////////////////////////////////////////////////////////////////////////
	class CDecoratorNode : public CBaheviorTreeFunctionNode
	{
	public:
		CDecoratorNode();
		virtual ~CDecoratorNode();
		virtual NodeStatus execute() final;
		DeclareScriptClass(CDecoratorNode);
	};


	//////////////////////////////////////////////////////////////////////////
	class CRepeatNode : public CBaheviorTreeFunctionNode
	{
	public:
		CRepeatNode();
		virtual ~CRepeatNode();
		virtual NodeStatus execute() final;
		DeclareScriptClass(CRepeatNode);
	};
}