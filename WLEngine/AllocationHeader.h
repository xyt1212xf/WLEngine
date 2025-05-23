#pragma once
#include "AllocatorLabels.h"
#include "TypeUtilties.h"
#include "MemoryProfiler.h"
#include "Tlsf.h"

namespace WL
{
	class CNullAllocationSizeHeader
	{
	public:
		void   initAllocationSizeHeader([[maybe_unused]] void* allocPtr, [[maybe_unused]] size_t padCount, [[maybe_unused]] size_t size) {}
		bool   hasPadding() const { return false; }
		void   setPaddingCount([[maybe_unused]] size_t padding) {}
		size_t getPaddingCount() const { return 0; }
	};

	class CAllocationSizeHeader
	{
	public:
		void   initAllocationSizeHeader(void* allocPtr, size_t padCount, size_t size);
		bool   hasPadding() const;
		size_t getAllocationSize() const;
		void   setPaddingCount(size_t padding);
		size_t getPaddingCount() const;
		
	protected:
		static const UINT8 kPadValue = 0xAA;
		size_t m_HasPadding : 1;
		size_t m_AllocationSize : ((sizeof(size_t) << 3) - 1);
	};

#pragma pack(push, 1)	
	class CMemoryDebuggingHeader
#if ENABLE_MEM_PROFILER
	: public CAllocationSizeHeader
#endif
	{
	public:
		UINT16 getAllocatorIdentifier() const
		{
			return m_AllocatorIdentifier; 
		}

		void initMemoryDebuggingHeader(void* footerPtr, int allocatorId)
		{
			m_AllocatorIdentifier = allocatorId;
			m_Magic = kMagicValue;
			// set footer
			memset(footerPtr, kFooterValue, kFooterSize);
		}

	protected:
		static const UINT32 kMagicValue = 0xDFA;
		static const UINT32 kFooterSize = 4;
		static const UINT8  kFooterValue = 0xFD;
		UINT16 m_AllocatorIdentifier;
		UINT16 m_Magic : 12;
	};
#pragma pack(pop)
	
	template<bool HasAllocationSize>
	struct SRealSizeHelper
	{
		template<class T>
		static size_t getRealAllocationSize(const T* header)
		{
			return header->getOverheadSize() + header->getSize() + header->getAllocationSize() + T::kFooterSize;
		}
	};

	template<typename T>
	class THasAllocationSize
	{
	private:
		typedef struct { char m; } yes;
		typedef struct { char m[2]; } no;
		struct BaseMixin
		{
			size_t getAllocationSize() const { return 0; };
		};
		struct Base : public T, public BaseMixin {};
		template <typename U, U> class CHelper {};
		template <typename U> static no deduce(U*, CHelper<size_t(BaseMixin::*)()const, &U::getAllocationSize>* = 0);
		static yes deduce(...);

	public:
		static const bool result = (sizeof(yes) == sizeof(deduce((Base*)(0))));
	};

	template<class T>
	class TAllocationHeaderBase :
		public CMemoryDebuggingHeader,
		public std::conditional<THasAllocationSize<CMemoryDebuggingHeader>::result, NullType, T>::type
	{
	public:
		///< Init header data
		static TAllocationHeaderBase<T>* init(void* allocPtr, int allocatorId, size_t size, size_t align)
		{
			DebugAssert(align > 0 && align <= 16 * 1024 && Foundation::isPowerOfTwo(align));

			size_t padCount = getRequiredPadding(allocPtr, align);
			TAllocationHeaderBase<T>* header = (TAllocationHeaderBase<T>*)((char*)allocPtr + padCount);
			header->BaseTypeWithPadding::initAllocationSizeHeader(allocPtr, padCount, size);
			header->initMemoryDebuggingHeader(((char*)header->getUserPtr()) + size, allocatorId);

			return header;
		}

		///< Return total size for allocation with all attached headers
		static size_t calculateNeededAllocationSize(size_t size, int align)
		{
			if (THasAllocationSize<TAllocationHeaderBase<T> >::result)
			{
				// If size info is included we need extra space for internal alignment
				int alignMask = align - 1;
				return alignMask + getSize() + size + kFooterSize;
			}
			else
			{
				return alignSize(size, align);
			}
		}

		static size_t getRequiredPadding(void* allocPtr, size_t userAlign)
		{
			if (THasAllocationSize<TAllocationHeaderBase<T> >::result)
			{
				return userAlign - ((((size_t)allocPtr + getSize() - 1) & (userAlign - 1)) + 1);
			}
			else
			{
				return 0;
			}
		}
		///< Return AllocationHeader ptr from user ptr
		static const TAllocationHeaderBase<T>* getAllocationHeader(const void* userPtr)
		{
			return (const TAllocationHeaderBase<T>*)((const char*)userPtr - getSize());
		}

		///< Return AllocationHeader size without profiler data size
		static size_t getSelfSize()
		{
			if (THasAllocationSize<TAllocationHeaderBase<T> >::result)
			{
				return sizeof(TAllocationHeaderBase<T>);
			}
			else
			{
				return 0;
			}
		}
		
		///< Return AllocationHeader data size
		static size_t getSize()
		{
			return getSelfSize()                            // Self size
				+ CMemoryProfiler::getHeaderSize();       // ProfilerAllocationHeader size
		}


		///< Return real ptr allocated by allocator
		void* getAllocationPtr() const
		{
			return (void*)((const char*)this - BaseTypeWithPadding::getPaddingCount());
		}
		void* getUserPtr() const
		{
			return (void*)((const char*)this + getSize());
		}

		SProfilerAllocationHeader* getProfilerHeader() const
		{
			return (SProfilerAllocationHeader*)((const char*)this + getSelfSize());
		}
				
		///< Return size without header size
		size_t adjustUserPtrSize(size_t size) const
		{
			return size - getSize() - BaseTypeWithPadding::getPaddingCount();
		}

		size_t getOverheadSize() const
		{
			if (THasAllocationSize<TAllocationHeaderBase<T> >::result)
			{
				return getSize() + kFooterSize + kDefaultMemoryAlignment - 1;  // Estimate
			}
			else
			{
				return getSize();
			}
		}

	protected:
		// Required for correct GetPaddingCount and InitAllocationSizeHeader functions resolving when using PSM compiler.
		typedef typename std::conditional<THasAllocationSize<CMemoryDebuggingHeader>::result, CMemoryDebuggingHeader, T>::type BaseTypeWithPadding;

		friend struct SRealSizeHelper<THasAllocationSize<BaseTypeWithPadding>::result>;
	};


	typedef TAllocationHeaderBase<CNullAllocationSizeHeader> CAllocationHeader;
	typedef TAllocationHeaderBase<CAllocationSizeHeader>     CAllocationHeaderWithSize;
}