#pragma once
#include <string>

namespace WL
{
	struct AllocationRootReference;

	struct ProfilerAllocationHeader
	{
		AllocationRootReference* rootReference = nullptr;
	};

	struct AllocationRootReference
	{
		AllocationRootReference(ProfilerAllocationHeader* header, size_t size, const char* areaName, const char* objectName);
		~AllocationRootReference();
		void setObjectName(const char* objectName);

		volatile size_t  m_AccumulatedSize = 0;
		const char* m_AreaName = nullptr;
		std::string   m_ObjectName;
	};

	class CMemoryProfiler
	{
	public:
		CMemoryProfiler();
		~CMemoryProfiler();
		static void staticInitialize();
		static void staticDestroy();
		static int getHeaderSize();
		void allocateStructs();

	public:
		static CMemoryProfiler* s_MemoryProfiler;
	};

	inline CMemoryProfiler* getMemoryProfiler()
	{
		return CMemoryProfiler::s_MemoryProfiler;
	}
}