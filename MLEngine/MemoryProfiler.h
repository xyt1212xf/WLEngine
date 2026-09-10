#pragma once
#include <string>

namespace WL
{
	struct SAllocationRootReference;

	struct SProfilerAllocationHeader
	{
		SAllocationRootReference* rootReference = nullptr;
	};

	struct SAllocationRootReference
	{
		SAllocationRootReference(SProfilerAllocationHeader* header, size_t size, const char* areaName, const char* objectName);
		~SAllocationRootReference();
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