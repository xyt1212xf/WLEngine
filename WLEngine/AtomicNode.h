#pragma once

#include "AtomicOps.h"
namespace WL
{
	class CAtomicNode
	{
		friend class AtomicStack;
		friend class AtomicQueue;
		friend class MutexLockedStack;
		friend class MutexLockedQueue;

		volatile atomic_word _next;

	public:
		void* data[3];

		CAtomicNode* next() const
		{
			return (CAtomicNode*)_next;
		}
		CAtomicNode* link(CAtomicNode* next);
	};

}