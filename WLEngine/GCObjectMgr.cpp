#include "GCObjectMgr.h"
#include "Object.h"
#include "DefMemory.h"

namespace WL
{
	ObjectHandle CGCObjectMgr::NextHandle = 0;
	CGCObjectMgr* CG = nullptr;
	CGCObjectMgr::CGCObjectMgr()
	{
		CG = this;
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
			FObjectHeader* Header = reinterpret_cast<FObjectHeader*>(reinterpret_cast<char*>(Obj) - ObjectHeadSize);
			RootSet.insert(Header->SelfHandle);
		}
	}

	// 从根集移除
	void CGCObjectMgr::RemoveFromRoot(CObject* Obj)
	{
		if (Obj)
		{
			FObjectHeader* Header = reinterpret_cast<FObjectHeader*>(reinterpret_cast<char*>(Obj) - ObjectHeadSize);
			RootSet.erase(Header->SelfHandle);
		}
	}

	void CGCObjectMgr::AddReference(CObject* Obj, CObject* Child)
	{
		FObjectHeader* Header = reinterpret_cast<FObjectHeader*>(reinterpret_cast<char*>(Obj) - ObjectHeadSize);
		FObjectHeader* ChildHeader = reinterpret_cast<FObjectHeader*>(reinterpret_cast<char*>(Child) - ObjectHeadSize);
		RefTable.AddReference(Header->SelfHandle, ChildHeader->SelfHandle);
	}

	size_t CGCObjectMgr::CollectGarbage()
	{
		MarkPhase();
		return SweepAndCompact();
	}

	FObjectHeader* CGCObjectMgr::GetHeader(ObjectHandle Handle)
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
		return reinterpret_cast<FObjectHeader*>(MemoryPool + Offset);
	}

	FObjectHeader* CGCObjectMgr::GetHeaderSafe(ObjectHandle Handle)
	{
		FObjectHeader* Header = GetHeader(Handle);
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
		FObjectHeader* Header = GetHeaderSafe(Handle);
		if (!Header || Header->bMarked)
		{
			return;
		}
		Header->bMarked = true;

		// 通过引用表遍历引用
		for (ObjectHandle Ref : RefTable.GetReferences(Header->SelfHandle)) 
		{
			MarkRecursive(Ref);
		}
	}

	void CGCObjectMgr::MarkPhase() 
	{
		// 1. 先清除所有对象的标记
		for (auto& [Handle, Offset] : HandleToOffset) 
		{
			FObjectHeader* Header = reinterpret_cast<FObjectHeader*>(MemoryPool + Offset);
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
		std::sort(SortedHandles.begin(), SortedHandles.end());
		for (ObjectHandle Handle : SortedHandles) 
		{
			FObjectHeader* Header = GetHeader(Handle);
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
					if (Header->SelfHandle != INVALID_HANDLE)
					{
						RefTable.FreeRow(Header->SelfHandle);
					}
				}
				CObject* Obj = reinterpret_cast<CObject*>(MemoryPool + HandleToOffset[Handle] + ObjectHeadSize);
				Obj->~CObject();
				AliveHandles.erase(Handle);
			}
		}

		// 2. 压缩引用表
		RefTable.Compact(AliveHandles, HandleToNewRowIndex,
			[this](ObjectHandle Handle) -> UINT32 {
				FObjectHeader* Header = GetHeader(Handle);
				return Header ? Header->SelfHandle : INVALID_HANDLE;
			});

		// 3. 移动存活对象到新位置
		std::unordered_map<ObjectHandle, size_t> NewHandleToOffset;
		for (auto& [OldOff, NewOff] : MovePlan) 
		{
			if (OldOff != NewOff) 
			{
				FObjectHeader* Header = reinterpret_cast<FObjectHeader*>(MemoryPool + OldOff);
				memmove(MemoryPool + NewOff, MemoryPool + OldOff, ObjectHeadSize + Header->ObjectSize);
			}

			// 更新对象头中的引用表索引
			FObjectHeader* Header = reinterpret_cast<FObjectHeader*>(MemoryPool + NewOff);
			// 保存新的偏移映射
			NewHandleToOffset[Header->SelfHandle] = NewOff;
		}

		// 4. 替换映射表
		HandleToOffset = std::move(NewHandleToOffset);
		PoolUsed = NewOffset;

		return SortedHandles.size() - AliveHandles.size();
	}


	void CReferenceTable::FreeRow(UINT32 RowIndex)
	{
		References[RowIndex].clear();
		References[RowIndex].shrink_to_fit();
	}


	void CReferenceTable::AddReference(UINT32 RowIndex, ObjectHandle Target)
	{
		References[RowIndex].emplace_back(Target);
	}

	const std::vector<WL::ObjectHandle>& CReferenceTable::GetReferences(UINT32 RowIndex) const
	{
		static const std::vector<ObjectHandle> Empty;
		auto It = References.find(RowIndex);
		if (It != References.end())
		{
			return It->second;
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
									const std::function<UINT32(ObjectHandle)>& GetOldIndexFunc)
	{
		if (References.size() == 0)
		{
			return;
		}
		std::unordered_map<UINT32, std::vector<ObjectHandle>> NewReferences;
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
				NewReferences[NewIndex].resize(NewIndex + 1);
			}
			NewReferences[NewIndex] = std::move(FilteredRefs);
		}

		References = std::move(NewReferences);
	}

}