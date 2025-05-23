#pragma once
#include <limits>
#include "DefaultAllocator.h"
#include "BaseAllocator.h"
#include "Mutex.h"

//	These methods are added to be able to make some initial allocations that does not use the memory manager
namespace WL
{
	/// Return maximum alignment. Both alignments must be 2^n.
	INLINE size_t maxAlignment(size_t alignment1, size_t alignment2)
	{
		return ((alignment1 - 1) | (alignment2 - 1)) + 1;
	}
	INLINE bool isTempAllocatorLabel(MemLabelRef label)
	{
		return getLabelIdentifier(label) == kMemTempAllocId; 
	}
	class CMemoryMgr
	{
		struct SLabelInfo
		{
			CBaseAllocator* pAlloc = nullptr;
#ifdef WL_64
			ALIGN_TYPE(8) volatile size_t allocatedMemory;
			ALIGN_TYPE(8) volatile size_t largestAlloc;
			ALIGN_TYPE(4) volatile int    numAllocs;
#else
			ALIGN_TYPE(4) volatile size_t allocatedMemory;
			ALIGN_TYPE(4) volatile size_t largestAlloc;
			ALIGN_TYPE(4) volatile int    numAllocs;
#endif // UNITY_64
		};
		SLabelInfo        m_AllocatorMap[kMemLabelCount];
	public:
		CMemoryMgr();
		virtual ~CMemoryMgr();
		void* allocate(size_t size, int align, MemLabelRef label, int allocateOptions = kAllocateOptionNone, const char* file = nullptr, int line = 0);
		void* reallocate(void* ptr, size_t size, int align, MemLabelRef label, int allocateOptions = kAllocateOptionNone, const char* file = NULL, int line = 0);
		void  deallocate(void* ptr, MemLabelRef label);
		void  deallocate(void* ptr);
		void delMemory(void* p, CBaseAllocator* pPool = nullptr);
		void* newMemory(size_t size, CBaseAllocator* pPool = nullptr);
		void threadInitialize(size_t tempSize = 0);
		void threadCleanup();
		CBaseAllocator* getAllocator(MemLabelRef label);
		void startLoggingAllocations(size_t logAllocationsThreshold = 0);
		void stopLoggingAllocations();
		bool isActive() const;
		void checkDisalowAllocation();

		static void* lowLevelAllocate(size_t size);
		static void* lowLevelCAllocate(size_t count, size_t size);
		static void* lowLevelReallocate(void* p, size_t size, size_t oldSize);
		static void  lowLevelFree(void* p, size_t oldSize);
	public:
		volatile static size_t m_LowLevelAllocated;
		volatile static size_t m_RegisteredGfxDriverMemory;

	protected:
		bool initialize();
		void unInitialize();

	private:
		bool   initializeMainThreadAllocators();
		void   initializeDefaultAllocators();
		bool   initializeDebugAllocator();
#if ENABLE_MEM_PROFILER 
		void registerAllocation(void* ptr, size_t size, MemLabelRef label, const char* function, const char* file, int line);
		void registerDeallocation(void* ptr, MemLabelRef label, const char* function);
#endif
	private:
		static const size_t kMaxAllocators = 16;
		static const size_t kMaxCustomAllocators = 512;
		CMutex  mMutex;
		CBaseAllocator* m_InitialFallbackAllocator = nullptr;
		CBaseAllocator* m_FrameTempAllocator = nullptr;
		CBaseAllocator* m_BucketAllocator = nullptr;

		CBaseAllocator* m_CustomAllocators[kMaxCustomAllocators];
		CBaseAllocator* m_MainAllocators[kMaxAllocators];
		CBaseAllocator* m_ThreadAllocators[kMaxAllocators];
		CBaseAllocator* m_Allocators[kMaxAllocators];
		
		size_t           m_LogAllocationsThreshold = 0;
		size_t           m_NextFreeCustomAllocatorIndex = 0;
		int              m_InitialFallbackTempAllocationsCount = 0;
		int		m_NumAllocators = 0;
		int     m_PS3DelayedReleaseIndex;
		bool    m_LogAllocations = false;
		bool    m_IsInitialized = false;
		bool    m_IsActive = false;
		bool    m_IsInitializedDebugAllocator = false;
	};
	extern CMemoryMgr* GMemMgrPtr;
}

