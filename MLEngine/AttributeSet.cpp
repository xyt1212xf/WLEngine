#include "AttributeSet.h"
#include "WLEngine.h"

namespace WL
{
	DefineScriptClass(CAttributeSet)
	CAttributeSet::CAttributeSet()
	{

	}

	CAttributeSet::~CAttributeSet()
	{

	}
	void CAttributeSet::registerClass()
	{
		CRegisterClass<CAttributeSet>::registerClass(GEngine->getLuaState());
	}

	void CAttributeSet::registerMember()
	{
		auto pLua = GEngine->getLuaState();
		CRegisterFun<void>::registerClassMemberFun<CAttributeSet, void(CAttributeSet::*)(), &CAttributeSet::doBreak>(pLua, "doBreak");
	}
}