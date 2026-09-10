#include "Material.h"
namespace WL
{
	CMaterial::CMaterial()
	{

	}

	CMaterial::~CMaterial()
	{
		for (auto item : mpShaders)
		{
			WL_DECREASE(item);
		}
		mpShaders.clear();
		//for (auto item : mpTextures)
		//{
		//	SafeRelease(item);
		//}
		//mpTextures.clear();
		for (auto item : mSampleState)
		{
			DEV_RELEASE(item);
		}
		mSampleState.clear();
	}

}
