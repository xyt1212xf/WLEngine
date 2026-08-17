#include "TestGameFunc.h"
#include "GCObjectMgr.h"
#include "Object.h"
namespace WL
{
	void GCTest()
	{
		CObject* Child = WL_NEW(CObject, Object)("Child");
		CObject* GrandChild = WL_NEW(CObject, Object)("GrandChild");
		CObject* Parent = WL_NEW(CObject, Object)("Parent");
		CGCObjectMgr::getSinglePtr()->AddToRoot(Parent);

		// 建立引用链（模拟 UPROPERTY）
		Parent->AddReference(Child);
		Child->AddReference(GrandChild);

		CGCObjectMgr::getSinglePtr()->CollectGarbage();
	}
}