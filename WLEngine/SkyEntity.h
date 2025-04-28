#pragma once
#include "ActorEntity.h"
#include "MaterialInstance.h"
namespace WL
{
	class CSkyEntity : public CActorEntity
	{
	public:
		CSkyEntity();
		virtual ~CSkyEntity();
		bool initialise();

	private:
		virtual void _tick(UINT32 dt) final;

	private:
		CMaterialInstance* mpMaterialInst = nullptr;
		Vec4F			mEyePosition = {0, 0, 0, 1};
		Vec3F			mSunDirection = {0, 1, 0};
		float			mSkyHeight = 2000.0f;
		shaderParam	mEyePositionHandle;
		shaderParam	mSunDirectionHandle;
		shaderParam	mSkyHeightHandle;
	};
}

