#include "GCObjectMgr.h"
#include "Object.h"
#include "DefMemory.h"

namespace WL
{
	ObjectHandle CGCObjectMgr::NextHandle = 0;

	CGCObjectMgr::CGCObjectMgr()
	{
		MemoryPool = reinterpret_cast<char*>(WL_MALLOC(MemLabelRef(kMemObjectId), PoolCapacity * 2));
	}

	CGCObjectMgr::~CGCObjectMgr()
	{
		//for (CObject* Obj : AllObjects) 
		//{
		//	WL_DELETE(Obj, Object);
		//}
		//AllObjects.clear();
	}

	void CGCObjectMgr::AddToRoot(CObject* Obj)
	{
		if (Obj) 
		{
			ObjectHeader* Header = reinterpret_cast<ObjectHeader*>(reinterpret_cast<char*>(Obj) - ObjectHeadSize);
			RootSet.insert(Header->SelfHandle);
		}
	}

	// 从根集移除
	void CGCObjectMgr::RemoveFromRoot(CObject* Obj)
	{
		//RootSet.erase(Obj);
	}

	void CGCObjectMgr::InsertObject(CObject* Obj)
	{
		//AllObjects.insert(Obj);
	}


	size_t CGCObjectMgr::CollectGarbage()
	{
		MarkPhase();
		return SweepAndCompact();
	}

	ObjectHeader* CGCObjectMgr::GetHeader(ObjectHandle Handle)
	{
		auto It = HandleToOffset.find(Handle);
		if (It == HandleToOffset.end()) 
		{
			return nullptr;  // 句柄不存在或已被回收
		}
		UINT32 Offset = It->second;
		if (Offset >= PoolUsed) 
		{
			return nullptr;  // 偏移越界（不应该发生）
		}
		return reinterpret_cast<ObjectHeader*>(MemoryPool + Offset);
	}

	ObjectHeader* CGCObjectMgr::GetHeaderSafe(ObjectHandle Handle)
	{
		ObjectHeader* Header = GetHeader(Handle);
		if (!Header)
		{
			return nullptr;
		}
		// 验证头部的 SelfHandle 是否匹配（防止内存损坏）
		if (Header->SelfHandle != Handle) 
		{
			return nullptr;
		}
		return Header;
	}

	void CGCObjectMgr::MarkRecursive(ObjectHandle Handle)
	{
		ObjectHeader* Header = GetHeaderSafe(Handle);
		if (!Header || Header->bMarked)
		{
			return;
		}
		Header->bMarked = true;

		// 通过引用表遍历引用
		for (ObjectHandle Ref : RefTable.GetReferences(Header->ReferenceTableIndex)) 
		{
			MarkRecursive(Ref);
		}
	}

	void CGCObjectMgr::MarkPhase() 
	{
		// 1. 先清除所有对象的标记
		for (auto& [Handle, Offset] : HandleToOffset) 
		{
			ObjectHeader* Header = reinterpret_cast<ObjectHeader*>(MemoryPool + Offset);
			Header->bMarked = false;
		}

		// 从根集开始标记
		for (ObjectHandle Root : RootSet) 
		{
			MarkRecursive(Root);
		}
	}

	size_t CGCObjectMgr::SweepAndCompact() 
	{
		std::vector<std::pair<size_t, size_t>> MovePlan; // {old_offset, new_offset}
		std::unordered_map<ObjectHandle, size_t> HandleToNewRowIndex;
		size_t NewOffset = 0;
		size_t NewRowIndex = 0;

		// 遍历当前所有对象（按句柄排序以保证确定性）
		std::vector<ObjectHandle> SortedHandles;
		for (const auto& [Handle, _] : HandleToOffset) 
		{
			SortedHandles.emplace_back(Handle);
		}
		
		for (ObjectHandle Handle : SortedHandles) 
		{
			ObjectHeader* Header = GetHeader(Handle);
			if (Header && Header->bMarked) 
			{
				// 存活对象：记录移动计划
				MovePlan.emplace_back(HandleToOffset[Handle], NewOffset);
				HandleToNewRowIndex[Handle] = NewRowIndex++;
				NewOffset += Header->ObjectSize + ObjectHeadSize;
			}
			else 
			{
				// 死亡对象：清理
				if (Header) 
				{
					RefTable.FreeRow(Header->ReferenceTableIndex);
				}
				CObject* Obj = reinterpret_cast<CObject*>(MemoryPool + HandleToOffset[Handle] + ObjectHeadSize);
				Obj->~CObject();
				AliveHandles.erase(Handle);
			}
		}

		// 2. 压缩引用表
		RefTable.Compact(AliveHandles, HandleToNewRowIndex,
			[this](ObjectHandle Handle) -> size_t {
				ObjectHeader* Header = GetHeader(Handle);
				return Header ? Header->ReferenceTableIndex : 0;
			});

		// 3. 移动存活对象到新位置
		std::unordered_map<ObjectHandle, size_t> NewHandleToOffset;
		for (auto& [OldOff, NewOff] : MovePlan) 
		{
			if (OldOff != NewOff) 
			{
				memmove(MemoryPool + NewOff, MemoryPool + OldOff, OldOff);
			}

			// 更新对象头中的引用表索引
			ObjectHeader* Header = reinterpret_cast<ObjectHeader*>(MemoryPool + NewOff);
			Header->ReferenceTableIndex = HandleToNewRowIndex[Header->SelfHandle];

			// 保存新的偏移映射
			NewHandleToOffset[Header->SelfHandle] = NewOff;
		}

		// 4. 替换映射表
		HandleToOffset = std::move(NewHandleToOffset);
		PoolUsed = NewOffset;

		return SortedHandles.size() - AliveHandles.size();
	}

	UINT32 CReferenceTable::AllocateRow()
	{
		UINT32 Index = INVALID_HANDLE;
		if (!FreeIndices.empty())
		{
			Index = FreeIndices.back();
			FreeIndices.pop_back();
			References[Index].clear();
		}
		else
		{
			Index = References.size();
			References.emplace_back();
		}
		return Index;
	}

	void CReferenceTable::FreeRow(UINT32 RowIndex)
	{
		References[RowIndex].clear();
		References[RowIndex].shrink_to_fit();
		FreeIndices.push_back(RowIndex);
	}


	void CReferenceTable::AddReference(UINT32 RowIndex, ObjectHandle Target)
	{
		if (RowIndex < References.size())
		{
			References[RowIndex].push_back(Target);
		}
	}

	const std::vector<WL::ObjectHandle>& CReferenceTable::GetReferences(UINT32 RowIndex) const
	{
		static const std::vector<ObjectHandle> Empty;
		if (RowIndex < References.size())
		{
			return References[RowIndex];
		}
		return Empty;
	}

	void CReferenceTable::ClearReferences(UINT32 RowIndex)
	{
		if (RowIndex < References.size()) 
		{
			References[RowIndex].clear();
		}
	}

	void CReferenceTable::Compact(const std::unordered_set<ObjectHandle>& AliveObjects, 
									const std::unordered_map<ObjectHandle, size_t>& HandleToNewRowIndex,
									const std::function<size_t(ObjectHandle)>& GetOldIndexFunc)
	{
		std::vector<std::vector<ObjectHandle>> NewReferences;
		NewReferences.reserve(AliveObjects.size());

		// 按新索引顺序重建引用表
		for (const auto& [Handle, NewIndex] : HandleToNewRowIndex) 
		{
			UINT32 OldIndex = GetOldIndexFunc(Handle);  
			auto& OldRefs = References[OldIndex];

			std::vector<ObjectHandle> FilteredRefs;
			for (ObjectHandle Ref : OldRefs) 
			{
				// 只保留指向存活对象的引用
				if (AliveObjects.count(Ref)) 
				{
					FilteredRefs.emplace_back(Ref);
				}
			}

			if (NewIndex >= NewReferences.size()) 
			{
				NewReferences.resize(NewIndex + 1);
			}
			NewReferences[NewIndex] = std::move(FilteredRefs);
		}

		References = std::move(NewReferences);
		FreeIndices.clear();
	}

}