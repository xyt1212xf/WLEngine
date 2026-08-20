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
		void FreeRow(UINT32 RowIndex);
		void AddReference(UINT32 RowIndex, ObjectHandle Target);
		const std::vector<ObjectHandle>& GetReferences(UINT32 RowIndex) const;
		void ClearReferences(UINT32 RowIndex);
		void Compact(const std::unordered_set<ObjectHandle>& AliveObjects, 
					 const std::unordered_map<ObjectHandle, size_t>& HandleToNewRowIndex,
					 const std::function<UINT32(ObjectHandle)>& GetOldIndexFunc);

	private:
		std::unordered_map<UINT32, std::vector<ObjectHandle>> References;
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
				
		void AddReference(CObject* Obj, CObject* Child);

		size_t CollectGarbage();
		
		FObjectHeader* GetHeader(ObjectHandle Handle) ;
		FObjectHeader* GetHeaderSafe(ObjectHandle Handle);

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

	extern CGCObjectMgr* GC;
	template<typename T>
	T* NewObject(const std::string& Name)
	{
		T* Object = nullptr;
		if (GC != nullptr)
		{
			UINT32 _ObjectSize = sizeof(T);
			if (GC->PoolUsed + ObjectHeadSize + _ObjectSize > GC->PoolCapacity)
			{
				GC->CollectGarbage();
				WL_FREE(MemLabelRef(kMemObjectId), GC->MemoryPool);
				GC->MemoryPool = reinterpret_cast<char*>(WL_MALLOC(MemLabelRef(kMemObjectId), GC->PoolCapacity * 2));
				GC->PoolCapacity *= 2;
			}
		
			size_t Offset = GC->PoolUsed;
			GC->PoolUsed += ObjectHeadSize + _ObjectSize;
			FObjectHeader* Header = reinterpret_cast<FObjectHeader*>(GC->MemoryPool + Offset);
			memset(Header, 0, ObjectHeadSize);
			Header->SelfHandle = CGCObjectMgr::NextHandle++;
			strncpy_s(Header->Name, Name.c_str(), sizeof(Header->Name) - 1);
			Header->ObjectSize = _ObjectSize;
			GC->HandleToOffset[Header->SelfHandle] = Offset;
			GC->AliveHandles.insert(Header->SelfHandle);
			Object = new (GC->MemoryPool + Offset + ObjectHeadSize)T(Name);
		}
		return Object;
	}
}


