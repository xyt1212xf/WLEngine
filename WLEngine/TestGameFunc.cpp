#include "TestGameFunc.h"
#include "GCObjectMgr.h"
#include "Object.h"
#include "DefMemory.h"
#include "GCObjectMgr.h"

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



		CObject* A = NewObject<CObject>("A");
		CObject* B = NewObject<CObject>("B");
		CObject* C = NewObject<CObject>("C");
		CObject* D = NewObject<CObject>("D");
		CGCObjectMgr::getSinglePtr()->AddToRoot(B);
		CGCObjectMgr::getSinglePtr()->AddToRoot(D);
		CGCObjectMgr::getSinglePtr()->CollectGarbage();
	}
}