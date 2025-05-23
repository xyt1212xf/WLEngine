#include "MemoryMgr.h"
#include "MemoryCookie.h"
#include "WLEngine.h"
#include "DynamicHeapAllocator.h"
#include "LowLevelDefaultAllocator.h"
#include "DualThreadAllocator.h"
#include "ThreadsafeLinearAllocator.h"
#include "TLSAllocator.h"
#include "StackAllocator.h"
#include "BucketAllocator.h"
#include "MemoryProfiler.h"
#include "DefMemory.h"
#include "Thread.h"

#define USE_DEBUG_ALLOCATOR 1
#define WL_LL_ALLOC(l,s,a) ::_aligned_malloc(s, a)
#define WL_LL_REALLOC(l,p,s,a) ::_aligned_realloc(p, s, a)
#define WL_LL_FREE(l,p) ::_aligned_free(p)
#ifdef _WINDOWS
	#include <winnt.h>
	#define ON_WIN(x) x
#else
	#define ON_WIN(x)
#endif

namespace WL
{
	//static const size_t gs_stCookieSize = ROUND_UP(sizeof(CMemoryCookie), 8);
	//static const size_t gs_stCheckSize = 1;
	const size_t kMaxAllocatorOverhead = 64 * 1024;
	
	volatile size_t CMemoryMgr::m_LowLevelAllocated = 0;
	volatile size_t CMemoryMgr::m_RegisteredGfxDriverMemory = 0;

	static const size_t kDynamicHeapChunkSize = 16 * 1024 * 1024;
	static const size_t kTempAllocatorMainSize = 16 * 1024 * 1024;
	static const size_t kTempAllocatorThreadSize = 256 * 1024;

	typedef TLSAllocator<CStackAllocator> TempTLSAllocator;
	typedef CDualThreadAllocator< TDynamicHeapAllocator< CLowLevelAllocator > > MainThreadAllocator;

	void OutOfMemoryError(size_t size, int align, MemLabelRef label, int line, const char* file)
	{
		std::string str;
		str.reserve(30 * 1024);
		/*
		str += FormatString<std::string>("Could not allocate memory: System out of memory!\n");
		str += FormatString<std::string>("Trying to allocate: %" PRINTF_SIZET_FORMAT "B with %d alignment. MemoryLabel: %s\n", size, align, GetMemoryManager().GetMemcatName(label));
		str += FormatString<std::string>("Allocation happend at: Line:%d in %s\n", line, file);
		PrintShortMemoryStats(str, label);
		// first call the plain printf_console, to make a printout that doesn't do callstack and other allocations
		printf_console("%s", str.c_str());
		// Then do a FatalErroString, that brings up a dialog and launches the bugreporter.
		FatalErrorString(str.c_str());
		*/
	}


	INLINE void* checkAllocation(void* ptr, size_t size, int align, MemLabelRef label, const char* file, int line)
	{
		if (nullptr == ptr)
		{
			OutOfMemoryError(size, align, label, line, file);
		}
		return ptr;
	}

	CMemoryMgr* GMemMgrPtr = nullptr;
	void CMemoryMgr::deallocate(void* ptr, MemLabelRef label)
	{

	}

	void CMemoryMgr::deallocate(void* ptr)
	{

	}

	void CMemoryMgr::delMemory(void* p, CBaseAllocator* pPool /*= NULL*/)
	{

	}

	void* CMemoryMgr::newMemory(size_t size, CBaseAllocator* pPool /*= NULL*/)
	{
		return nullptr;
		//stSize += gs_stCookieSize + gs_stCheckSize;

		//uint8* const p = reinterpret_cast<uint8*>(NedMalloc(stSize, pPool));

		//CMemoryCookie* const pCookie = reinterpret_cast<CMemoryCookie*>(p);
		//pCookie->m_stSize = stSize;

		//if (CMemThreadMgr::Inst())
		//{
		//	pCookie->m_pStackInfo = new CMemCallStackInfo();

		//	CMemThreadMgr::Inst()->AddMemMallocJob(pCookie);
		//}
		//else
		//{
		//	pCookie->m_uCheckSum = pCookie->m_stSize ^ size_t(pCookie) ^ size_t(0xffffffffffffffffll);
		//}


		//uint8* const pBottomCheck = p + stSize - gs_stCheckSize;

		//*pBottomCheck = 0xbb;

		//return p + gs_stCookieSize;
	}

	void CMemoryMgr::threadInitialize(size_t tempSize /*= 0*/)
	{
		int tempAllocatorSize = kTempAllocatorThreadSize;
		const bool isMainThread = CThread::CurrentThreadIsMainThread();
		if (isMainThread && !m_IsInitialized)
		{
			initializeMainThreadAllocators();
			tempAllocatorSize = kTempAllocatorMainSize;
		}

		if (!m_IsInitializedDebugAllocator)
		{
			if (tempSize != 0)
				tempAllocatorSize = tempSize;
			// On a main thread we allow kMemTempJobAlloc allocations as they must be short-lived.
			// Other threads might live for longer and steal space from a renderer.
			const SMemLabelId fallbackLabel = isMainThread ? SMemLabelId(kMemTempJobAllocId) : SMemLabelId(kMemTempOverflowId);
			CStackAllocator* tempAllocator = WL_NEW(CStackAllocator(tempAllocatorSize, fallbackLabel, "ALLOC_TEMP_THREAD"), Manager);
			m_FrameTempAllocator->threadInitialize(tempAllocator);
		}
	}

	void CMemoryMgr::threadCleanup()
	{

	}

	CBaseAllocator* CMemoryMgr::getAllocator(MemLabelRef label)
	{
		auto identifier = getLabelIdentifier(label);
		if (identifier < kMemLabelCount)
		{
			CBaseAllocator* pAlloc = m_AllocatorMap[identifier].pAlloc;
			return pAlloc;
		}
		else
		{
			int index = identifier - kMemLabelCount;
			return m_CustomAllocators[index] <= (void*)kMaxCustomAllocators ? nullptr : m_CustomAllocators[index];
		}
	}

	void CMemoryMgr::startLoggingAllocations(size_t logAllocationsThreshold /*= 0*/)
	{
		m_LogAllocations = true;
		m_LogAllocationsThreshold = logAllocationsThreshold;
	}

	void CMemoryMgr::stopLoggingAllocations()
	{
		m_LogAllocations = false;
	}

	bool CMemoryMgr::isActive() const
	{
		return m_IsActive; 
	}

	void CMemoryMgr::checkDisalowAllocation()
	{

	}

	void* CMemoryMgr::lowLevelAllocate(size_t size)
	{
		void* ptr = WL_LL_ALLOC(kMemDefaultId, size, kDefaultMemoryAlignment);
		if (ptr != NULL)
		{
			ON_WIN(atomicAdd(&m_LowLevelAllocated, size));
		}
		return ptr;
	}

	void* CMemoryMgr::lowLevelCAllocate(size_t count, size_t size)
	{
		void* ptr = nullptr;
		return ptr;

	}

	void* CMemoryMgr::lowLevelReallocate(void* p, size_t size, size_t oldSize)
	{
		void* ptr = nullptr;
		return ptr;

	}

	void CMemoryMgr::lowLevelFree(void* p, size_t oldSize)
	{

	}

	bool CMemoryMgr::initialize()
	{
		memset(m_Allocators, 0, sizeof(m_Allocators));
		memset(m_MainAllocators, 0, sizeof(m_MainAllocators));
		memset(m_ThreadAllocators, 0, sizeof(m_ThreadAllocators));
		memset(m_AllocatorMap, 0, sizeof(m_AllocatorMap));

		//CBaseAllocator* defaultAllocator = m_Allocators[m_NumAllocators] = HEAP_NEW(CBaseAllocator)("ALLOC_DEFAULT");
		m_InitialFallbackAllocator = HEAP_NEW(TDynamicHeapAllocator<CLowLevelAllocator>) (1024 * 1024, 0, true, nullptr, "ALLOC_FALLBACK");

		m_NextFreeCustomAllocatorIndex = 0;
		for (intptr_t i = 0; i < kMaxCustomAllocators; i++)
		{
			m_CustomAllocators[i] = (CBaseAllocator*)(i + 1);
		}
		for (int i = 0; i < kMemLabelCount; i++)
		{
			m_AllocatorMap[i].pAlloc = m_InitialFallbackAllocator;
		}
		m_PS3DelayedReleaseIndex = -1;

		return initializeMainThreadAllocators();
	}

	void CMemoryMgr::unInitialize()
	{

	}

	bool CMemoryMgr::initializeMainThreadAllocators()
	{
		// We MUST not have any allocations labeled by kMemTempAlloc at the initialization point
		Assert(m_InitialFallbackTempAllocationsCount == 0);

		// Check if we force DebugAllocator initialization
		if (USE_DEBUG_ALLOCATOR)
		{
			m_IsInitializedDebugAllocator = initializeDebugAllocator();
			AssertMsg(m_IsInitializedDebugAllocator, "Unable to initialize DebugAllocator. Guarded allocations are disabled!");
		}

		// Use default allocators overwise
		if (!m_IsInitializedDebugAllocator)
		{
			initializeDefaultAllocators();
		}
		// Always create TempTLSAllocator to avoid branching in release players
		m_AllocatorMap[kMemTempAllocId].pAlloc = m_FrameTempAllocator = HEAP_NEW(TempTLSAllocator)("ALLOC_TEMP_THREAD");
		m_Allocators[m_NumAllocators++] = m_FrameTempAllocator;

		m_IsInitialized = true;
		m_IsActive = true;

#if ENABLE_MEM_PROFILER
		CMemoryProfiler::staticInitialize();
#endif
		return true;
	}

	void CMemoryMgr::initializeDefaultAllocators()
	{
		CBucketAllocator* bucketAllocator = nullptr;
		static const int kBucketAllocatorGranularity = 16;
		static const int kBucketAllocatorBucketsCount = 8;

		static const int kBucketAllocatorLargeBlockSize = 32 * 1024 * 1024;
		static const int kBucketAllocatorLargeBlocksCount = 4;

		m_BucketAllocator = bucketAllocator = HEAP_NEW(CBucketAllocator)("ALLOC_BUCKET",
			kBucketAllocatorGranularity, kBucketAllocatorBucketsCount, kBucketAllocatorLargeBlockSize, kBucketAllocatorLargeBlocksCount);

		m_MainAllocators[m_NumAllocators] = HEAP_NEW(TDynamicHeapAllocator<CLowLevelAllocator>) (kDynamicHeapChunkSize, 1024, false, NULL, "ALLOC_DEFAULT_MAIN");
		m_ThreadAllocators[m_NumAllocators] = HEAP_NEW(TDynamicHeapAllocator<CLowLevelAllocator>) (1024 * 1024, 1024, true, NULL, "ALLOC_DEFAULT_THREAD");
		CBaseAllocator* defaultAllocator = m_Allocators[m_NumAllocators] = HEAP_NEW(MainThreadAllocator)("ALLOC_DEFAULT", bucketAllocator, m_MainAllocators[m_NumAllocators], m_ThreadAllocators[m_NumAllocators]);
		CBaseAllocator* defaultThreadAllocator = m_ThreadAllocators[m_NumAllocators];
		m_NumAllocators++;

		for (int i = 0; i < kMemLabelCount; i++)
		{
			m_AllocatorMap[i].pAlloc = defaultAllocator;
		}

		m_AllocatorMap[kMemTempJobAllocId].pAlloc = m_Allocators[m_NumAllocators++] = HEAP_NEW(CThreadsafeLinearAllocator) (1 * 1024 * 1024, 64, "ALLOC_TEMP_JOB");

		m_MainAllocators[m_NumAllocators] = HEAP_NEW(TDynamicHeapAllocator<CLowLevelAllocator>) (kDynamicHeapChunkSize, 0, false, NULL, "ALLOC_GFX_MAIN");
		m_ThreadAllocators[m_NumAllocators] = HEAP_NEW(TDynamicHeapAllocator<CLowLevelAllocator>) (1024 * 1024, 0, true, NULL, "ALLOC_GFX_THREAD");
		CBaseAllocator* gfxAllocator = m_Allocators[m_NumAllocators] = HEAP_NEW(MainThreadAllocator)("ALLOC_GFX", bucketAllocator, m_MainAllocators[m_NumAllocators], m_ThreadAllocators[m_NumAllocators]);
		CBaseAllocator* gfxThreadAllocator = m_ThreadAllocators[m_NumAllocators];
		m_NumAllocators++;

		m_MainAllocators[m_NumAllocators] = HEAP_NEW(TDynamicHeapAllocator<CLowLevelAllocator>) (8 * 1024 * 1024, 0, false, NULL, "ALLOC_CACHEOBJECTS_MAIN");
		m_ThreadAllocators[m_NumAllocators] = HEAP_NEW(TDynamicHeapAllocator<CLowLevelAllocator>) (2 * 1024 * 1024, 0, true, NULL, "ALLOC_CACHEOBJECTS_THREAD");
		CBaseAllocator* cacheAllocator = m_Allocators[m_NumAllocators] = HEAP_NEW(MainThreadAllocator)("ALLOC_CACHEOBJECTS", bucketAllocator, m_MainAllocators[m_NumAllocators], m_ThreadAllocators[m_NumAllocators]);
		m_NumAllocators++;

		m_MainAllocators[m_NumAllocators] = HEAP_NEW(TDynamicHeapAllocator<CLowLevelAllocator>) (kDynamicHeapChunkSize, 0, false, NULL, "ALLOC_TYPETREE_MAIN");
		m_ThreadAllocators[m_NumAllocators] = HEAP_NEW(TDynamicHeapAllocator<CLowLevelAllocator>) (1024 * 1024, 0, true, NULL, "ALLOC_TYPETREE_THREAD");
		CBaseAllocator* typetreeAllocator = m_Allocators[m_NumAllocators] = HEAP_NEW(MainThreadAllocator)("ALLOC_TYPETREE", bucketAllocator, m_MainAllocators[m_NumAllocators], m_ThreadAllocators[m_NumAllocators]);
		m_NumAllocators++;

		m_MainAllocators[m_NumAllocators] = HEAP_NEW(TDynamicHeapAllocator<CLowLevelAllocator>) (4 * 1024 * 1024, 0, false, NULL, "ALLOC_PROFILER_MAIN");
		m_ThreadAllocators[m_NumAllocators] = HEAP_NEW(TDynamicHeapAllocator<CLowLevelAllocator>) (4 * 1024 * 1024, 0, true, NULL, "ALLOC_PROFILER_THREAD");
		CBaseAllocator* profilerAllocator = m_Allocators[m_NumAllocators] = HEAP_NEW(MainThreadAllocator)("ALLOC_PROFILER", bucketAllocator, m_MainAllocators[m_NumAllocators], m_ThreadAllocators[m_NumAllocators]);
		m_NumAllocators++;

		m_AllocatorMap[kMemDynamicGeometryId].pAlloc = gfxAllocator;
		m_AllocatorMap[kMemImmediateGeometryId].pAlloc = gfxAllocator;
		m_AllocatorMap[kMemSGeometryId].pAlloc = gfxAllocator;
		m_AllocatorMap[kMemVertexDataId].pAlloc = gfxAllocator;
		m_AllocatorMap[kMemBatchedGeometryId].pAlloc = gfxAllocator;
		m_AllocatorMap[kMemTextureId].pAlloc = gfxAllocator;

		m_AllocatorMap[kMemTypeTreeId].pAlloc = typetreeAllocator;

		m_AllocatorMap[kMemThreadId].pAlloc = defaultThreadAllocator;
		m_AllocatorMap[kMemGfxThreadId].pAlloc = gfxThreadAllocator;

		m_AllocatorMap[kMemTextureCacheId].pAlloc = cacheAllocator;
		m_AllocatorMap[kMemSerializationId].pAlloc = cacheAllocator;
		m_AllocatorMap[kMemFileId].pAlloc = cacheAllocator;

		m_AllocatorMap[kMemProfilerId].pAlloc = profilerAllocator;
		m_AllocatorMap[kMemMemoryProfilerId].pAlloc = profilerAllocator;
		m_AllocatorMap[kMemMemoryProfilerStringId].pAlloc = profilerAllocator;
	}

	bool CMemoryMgr::initializeDebugAllocator()
	{
		return false;
	}

	void CMemoryMgr::registerAllocation(void* ptr, size_t size, MemLabelRef label, const char* function, const char* file, int line)
	{
		if (getMemoryProfiler())
		{
		}
	}

	void CMemoryMgr::registerDeallocation(void* ptr, MemLabelRef label, const char* function)
	{

	}

	CMemoryMgr::CMemoryMgr()
	{
		GMemMgrPtr = this;
	}

	CMemoryMgr::~CMemoryMgr()
	{
		GMemMgrPtr = nullptr;

		for (int i = 0; i < m_NumAllocators; i++)
		{
			Assert(m_Allocators[i]->getAllocatedMemorySize() == 0);
		}

		threadCleanup();

		for (int i = 0; i < m_NumAllocators; i++)
		{
			HEAP_DELETE(m_Allocators[i], CBaseAllocator);
		}
		HEAP_DELETE(m_BucketAllocator, CBaseAllocator);
	}

	void* CMemoryMgr::allocate(size_t size, int align, MemLabelRef label, int allocateOptions, const char* file, int line)
	{
		if (0 == size)
		{
			size = 1;
		}
		void* ptr = nullptr;
		{
			CMutex::CAutoLock lock(mMutex);

			align = maxAlignment(align, kDefaultMemoryAlignment);
			if (doesAdditionOverflow(size, align + kMaxAllocatorOverhead))
			{
			}
			if (!isActive())
			{
				if (isTempLabel(label))
				{
					m_InitialFallbackTempAllocationsCount++;
				}

				ptr = m_InitialFallbackAllocator->allocate(size, align);
				return ptr;
			}

			if (isTempAllocatorLabel(label))
			{
				ptr = ((TempTLSAllocator*)m_FrameTempAllocator)->TempTLSAllocator::allocate(size, align);
				if (ptr != nullptr)
				{
					return ptr;
				}
				// If tempallocator thread has not been initialized fallback to kMemTempOverflow label
				return allocate(size, align, MemLabelRef(kMemTempOverflowId), allocateOptions, file, line);
			}

			CBaseAllocator* pAlloc = getAllocator(label);
			checkDisalowAllocation();
			ptr = pAlloc->allocate(size, align);

			if (nullptr != ptr && (allocateOptions & kAllocateOptionReturnNullIfOutOfMemory))
			{
				return nullptr;
			}
			checkAllocation(ptr, size, align, label, file, line);
		}
		// Register allocation in the profiler
		if (!isTempLabel(label))
		{
#if ENABLE_MEM_PROFILER
			registerAllocation(ptr, size, label, "Allocate", file, line);
#endif
			memset(ptr, 0xcd, size);
		}
		return ptr;
	}

	void* CMemoryMgr::reallocate(void* ptr, size_t size, int align, MemLabelRef label, int allocateOptions /*= kAllocateOptionNone*/, const char* file /*= NULL*/, int line /*= 0*/)
	{
		void* pData = nullptr;
		return pData;
	}

}

