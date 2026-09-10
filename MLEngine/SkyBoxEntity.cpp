#include "SkyBoxEntity.h"
#include "WLEngine.h"
namespace WL
{
	CSkyBoxEntity::CSkyBoxEntity()
	{
		mType = SkyBox;
	}

	CSkyBoxEntity::~CSkyBoxEntity()
	{
	}

	bool CSkyBoxEntity::initialise()
	{
		return true;
	}

}