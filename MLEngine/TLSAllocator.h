#pragma once
#include "BaseAllocator.h"

namespace WL
{
	template <class UnderlyingAllocator>
	class TLSAllocator : public CBaseAllocator
	{
	public:
		// when constructing it will be from the main thread
		TLSAllocator(const char* name)
		: CBaseAllocator(name)
		{
			if (s_NumberOfInstances != 0)
			{
				ErrorString("Only one instance of the TLS allocator is allowed because of TLS implementation");
			}
			s_NumberOfInstances++;
			memset(m_ThreadTempAllocators, 0, sizeof(m_ThreadTempAllocators));
		}
		virtual ~TLSAllocator()
		{
			--s_NumberOfInstances;
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

	private:
		static int s_NumberOfInstances;
		static const int      kMaxThreadTempAllocators = 128;
		UnderlyingAllocator* m_ThreadTempAllocators[kMaxThreadTempAllocators];
	};

	template <class UnderlyingAllocator>
	int TLSAllocator<UnderlyingAllocator>::s_NumberOfInstances = 0;

}
