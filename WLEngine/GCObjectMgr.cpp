#include "GCObjectMgr.h"

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

	// ´Ó¸ù¼¯ÒÆ³ý
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
		return 0;
	}

}