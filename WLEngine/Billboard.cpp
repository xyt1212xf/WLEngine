#include "Billboard.h"
#include "WLEngine.h"
namespace WL
{
	DefineScriptClass(CBillboard)

	CBillboard::CBillboard()
	{

	}

	CBillboard::~CBillboard()
	{

	}

	bool CBillboard::loadResource(const std::string& szFile)
	{
		bool bLoad = false;
		return bLoad;
	}

	void CBillboard::registerClass()
	{
		CRegisterClass<CBillboard>::registerClass(GEngine->getLuaState());
	}

	void CBillboard::registerMember()
	{
	}
}