#include "TestGameFunc.h"
#include "GCObjectMgr.h"
#include "Object.h"
#include "DefMemory.h"
#include "GCObjectMgr.h"

namespace WL
{
	void GCTest()
	{
		CObject* A = NewObject<CObject>("A");
		CObject* B = NewObject<CObject>("B");
		CObject* C = NewObject<CObject>("C");
		CObject* D = NewObject<CObject>("D");
		CG->AddToRoot(B);
		CG->AddReference(B, D);
		CG->CollectGarbage();
	}
}