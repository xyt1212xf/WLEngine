#include "TestGameFunc.h"
#include "GCObjectMgr.h"
#include "Object.h"
#include "DefMemory.h"
namespace WL
{
	void GCTest()
	{
		//CObject* Child = WL_NEW(CObject, Object)("Child");
		//CObject* GrandChild = WL_NEW(CObject, Object)("GrandChild");
		//CObject* Parent = WL_NEW(CObject, Object)("Parent");
		//CGCObjectMgr::getSinglePtr()->AddToRoot(Parent);

		//Parent->AddReference(Child);
		//Child->AddReference(GrandChild);

		CObject* pTest = WL_NEW(CObject, Object)("Test");


		CGCObjectMgr::getSinglePtr()->CollectGarbage();
	}
}