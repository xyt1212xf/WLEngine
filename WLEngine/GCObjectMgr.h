#pragma once

#include "TSingle.h"
#include "Common.h"
#include "DefMemory.h"
namespace WL
{
	enum EObjectFlags : int
	{
		RF_NoFlags					= 0x00000000,	
	};
	
	class CObject;
	class CClass;
	class CReferenceTable
	{
	public:
		UINT32 AllocateRow();
		void FreeRow(UINT32 RowIndex);
		void AddReference(UINT32 RowIndex, ObjectHandle Target);
		const std::vector<ObjectHandle>& GetReferences(UINT32 RowIndex) const;
		void ClearReferences(UINT32 RowIndex);
		void Compact(const std::unordered_set<ObjectHandle>& AliveObjects, 
					 const std::unordered_map<ObjectHandle, size_t>& HandleToNewRowIndex,
					 const std::function<size_t(ObjectHandle)>& GetOldIndexFunc);

	private:
		std::vector<std::vector<ObjectHandle>> References;
		std::vector<UINT32> FreeIndices;
	};

	class CGCObjectMgr : public TSingle<CGCObjectMgr>
	{
		template<typename U>
		friend U* NewObject(const std::string& Name);
	public:
		CGCObjectMgr();
		~CGCObjectMgr();

		void AddToRoot(CObject* Obj);
		// 从根集移除
		void RemoveFromRoot(CObject* Obj);
		
		void InsertObject(CObject* Obj);
		
		size_t CollectGarbage();
		
		ObjectHeader* GetHeader(ObjectHandle Handle) ;
		ObjectHeader* GetHeaderSafe(ObjectHandle Handle);

	private:
		void MarkRecursive(ObjectHandle Handle);
		void MarkPhase();
		size_t SweepAndCompact();

	private:
		size_t			PoolUsed = 0;
		size_t			PoolCapacity = 1024 * 1024;
		char*			MemoryPool = nullptr;
		CReferenceTable	RefTable;

		std::unordered_map<ObjectHandle, size_t> HandleToOffset;
	 
		std::unordered_set<ObjectHandle> AliveHandles;

		// 根集：永远存活的根对象
		std::unordered_set<ObjectHandle> RootSet;

		static ObjectHandle NextHandle;
	};


	template<typename T>
	T* NewObject(const std::string& Name)
	{
		T* Object = nullptr;
		if (CGCObjectMgr* CG = CGCObjectMgr::getSinglePtr())
		{
			UINT32 _ObjectSize = sizeof(T);
			if (CG->PoolUsed + ObjectHeadSize + _ObjectSize > CG->PoolCapacity)
			{
				CG->CollectGarbage();
				WL_FREE(MemLabelRef(kMemObjectId), CG->MemoryPool);
				CG->MemoryPool = reinterpret_cast<char*>(WL_MALLOC(MemLabelRef(kMemObjectId), CG->PoolCapacity * 2));
				CG->PoolCapacity *= 2;
			}
		
			size_t Offset = CG->PoolUsed;
			CG->PoolUsed += ObjectHeadSize + _ObjectSize;
			ObjectHeader* Header = reinterpret_cast<ObjectHeader*>(CG->MemoryPool + Offset);
			memset(Header, 0, ObjectHeadSize);
			Header->SelfHandle = CGCObjectMgr::NextHandle++;
			strncpy_s(Header->Name, Name.c_str(), sizeof(Header->Name) - 1);
			Header->ReferenceTableIndex = CG->RefTable.AllocateRow();
			Header->ObjectSize = _ObjectSize;
			CG->HandleToOffset[Header->SelfHandle] = Offset;
			CG->AliveHandles.insert(Header->SelfHandle);
			Object = new (CG->MemoryPool + Offset + ObjectHeadSize)T(Name);
		}
		return Object;
	}
}


