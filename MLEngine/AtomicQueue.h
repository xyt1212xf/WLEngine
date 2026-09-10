#pragma once
#include "AtomicNode.h"
namespace WL
{
	class CAtomicStack
	{
		volatile atomic_word2	_top;

	public:
		CAtomicStack();
		~CAtomicStack();
		int isEmpty() const;

		void push(CAtomicNode* node);
		void pushAll(CAtomicNode* first, CAtomicNode* last);

		CAtomicNode* pop();
		CAtomicNode* popAll();
	};
}