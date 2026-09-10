#pragma once
#include <stdlib.h>
#include <string>
#include <atomic>
#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#include <new>
#include "DefMemory.h"
#include "BaseAllocator.h"

namespace WL
{
	template<class LLAlloctor>
	class CDefaultAllocator : public CBaseAllocator
	{
	public:
		CDefaultAllocator(const char* szPoolName)
		: CBaseAllocator(szPoolName)
		{
			initPool(szPoolName);
		}
		virtual ~CDefaultAllocator()
		{

		}

		virtual void* allocate(size_t size, int align) final
		{
			void* ptr = nullptr;
			return ptr;
		}
		/// Reallocate memory block at the specified pointer.
		virtual void* reallocate(void* p, size_t size, int align) final
		{
			void* ptr = nullptr;
			return ptr;
		}

		/// Deallocate memory block.
		virtual void   deallocate(void* p) final
		{

		}

		void* getPool()
		{
			return m_pPool;
		}

	private:	
		void initPool(const char* szPoolName)
		{
			m_szPoolName = szPoolName;
		}

	private:
		std::string	m_szPoolName = "";
		void* m_pPool = nullptr;
		std::atomic<INT32> m_stMemUsage = 0;
		bool m_bCheckLeakage = true; 
	};
}