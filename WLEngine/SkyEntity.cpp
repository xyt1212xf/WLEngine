#include "SkyEntity.h"
#include "WLEngine.h"
namespace WL
{
	CSkyEntity::CSkyEntity()
	{
		mType = Sky;
	}

	CSkyEntity::~CSkyEntity()
	{
	}

	bool CSkyEntity::initialise()
	{
		auto iter = mModelInstanceMap.begin();
		auto pModelInstance = iter->second;
		mpMaterialInst = pModelInstance->getMaterialInstance();
		if (nullptr != pModelInstance)
		{
			mEyePositionHandle = mpMaterialInst->getVSParamByHandle("eyePosition");
			mSunDirectionHandle = mpMaterialInst->getVSParamByHandle("sunDirection");
			mSkyHeightHandle = mpMaterialInst->getVSParamByHandle("skyHeight");
			auto& v = getPosition();
			mEyePosition.x = v.x;
			mEyePosition.y = v.y;
			mEyePosition.z = v.z;
			return true;
		}
		else
		{
			return false;
		}
	}

	void CSkyEntity::_tick(UINT32 dt)
	{
		mpMaterialInst->setVSParamByName("eyePosition", &mEyePosition);
		mpMaterialInst->setVSParamByName("sunDirection", &mSunDirection);
		mpMaterialInst->setVSParamByName("skyHeight", &mSkyHeight);

		mpMaterialInst->setVSParamByHandle(mEyePositionHandle, &mEyePosition);

		//Vec4F a(1, 1, 0, 1);
		//mpMaterialInst->setPSParamByHandle(mTessPSHandle, &a);
	}

}