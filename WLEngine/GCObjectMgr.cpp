#include "GCObjectMgr.h"
#include "Object.h"
#include "DefMemory.h"

namespace WL
{

	CGCObjectMgr::~CGCObjectMgr()
	{
		for (CObject* Obj : AllObjects) 
		{
			WL_DELETE(Obj, Object);
		}
		AllObjects.clear();
	}

	void CGCObjectMgr::AddToRoot(CObject* Obj)
	{
		if (Obj) 
		{
			RootSet.insert(Obj);
		}
	}

	// 从根集移除
	void CGCObjectMgr::RemoveFromRoot(CObject* Obj)
	{
		RootSet.erase(Obj);
	}

	void CGCObjectMgr::InsertObject(CObject* Obj)
	{
		AllObjects.insert(Obj);
	}

	int CGCObjectMgr::CollectGarbage()
	{
		MarkPhase();
		return SweepPhase();
	}

	void CGCObjectMgr::MarkRecursive(CObject* Obj)
	{
		if (!Obj || Obj->bMarked) 
		{
			return;  // 已标记或空，返回
		}
		Obj->bMarked = true;               // 标记当前对象

		// 递归标记所有引用（深度优先）
		for (CObject* Ref : Obj->References) 
		{
			MarkRecursive(Ref);            // 沿着引用一路走到黑
		}
	}

	void CGCObjectMgr::MarkPhase() 
	{
		// 1. 先清除所有对象的标记
		for (CObject* Obj : AllObjects) 
		{
			Obj->bMarked = false;
		}

		// 2. 从根集开始标记
		for (CObject* Root : RootSet) 
		{
			if (Root) 
			{
				MarkRecursive(Root);
			}
		}
	}

	int CGCObjectMgr::SweepPhase() 
	{
		int FreedCount = 0;
		auto It = AllObjects.begin();
		while (It != AllObjects.end()) 
		{
			CObject* Obj = *It;
			if (!Obj->bMarked) 
			{
			 	WL_DELETE(Obj, Object);
				It = AllObjects.erase(It);
				FreedCount++;
			}
			else 
			{
				++It;
			}
		}
		return FreedCount;
	}
}