#pragma once
#include "TSingle.h"
#include "Object.h"
#include "Common.h"
#include "DefMemory.h"
//#include "AllocatorLabelNames.h"

namespace WL
{
	enum EObjectFlags : int
	{
		RF_NoFlags					= 0x00000000,	
	};
	
	class CObject;
	class CClass;
	
	class CGCObjectMgr : public TSingle<CGCObjectMgr>
	{
	public:
		CGCObjectMgr() = default;
		~CGCObjectMgr();

		void AddToRoot(CObject* Obj);
		// 从根集移除
		void RemoveFromRoot(CObject* Obj);
		
		void InsertObject(CObject* Obj);
		
		int CollectGarbage();

	private:
		// 所有被管理的对象
		std::unordered_set<CObject*> AllObjects;

		// 根集：永远存活的根对象
		std::unordered_set<CObject*> RootSet;
	};
}


