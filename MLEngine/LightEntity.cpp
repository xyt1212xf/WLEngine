#include "LightEntity.h"
#include "WLEngine.h"
#include "Scene.h"
namespace WL
{
	CLightModel::CLightModel()
	{
		mType = Actor;
		mszName = "LightModel";
	}

	CLightModel::~CLightModel()
	{

	}

	/////////////////////////////////////////////////////////////////////////
	CLightEntity::CLightEntity()
	{
		Math::matrixIdentity(&mViewMatrix);
		Math::matrixIdentity(&mProjectMatrix);		
		mData.fRange = 0.0001f;
	}

	void CLightEntity::bindModel(const std::string& szModel)
	{
		auto pModel = dynamic_cast<CModel*>(GEngine->createResource(szModel, Model));
		if (nullptr != pModel)
		{
			mpModelInstance = WL_NEW(CLightModel, Model);
			mpModelInstance->addModel(pModel);
			GEngine->getCurrentScenePtr()->addEntity(mpModelInstance);
			mpMaterialInstance = mpModelInstance->getModelInstance()->getMaterialInstance();
		}
	}

	CLightEntity::~CLightEntity()
	{
		WL_DECREASE(mpModelInstance);
	}

	void CLightEntity::setPosition(const Vec3F& vec)
	{
		mData.positionOrDirection = vec;
		CTranformComponet::setPosition(vec);
		if (nullptr != mpModelInstance)
		{
			mpModelInstance->setPosition(vec);
		}
	}

	const Vec3F& CLightEntity::getPosition() const
	{
		return CTranformComponet::getPosition();
	}

	void CLightEntity::setDirection(const Vec3F& dir)
	{
		mData.positionOrDirection = dir;
	}

	void CLightEntity::setColor(const Vec3F& color)
	{
		mData.color = color;
		if (nullptr != mpMaterialInstance)
		{
			mpMaterialInstance->setVertexColor(Vec4F(color.x, color.y, color.z, 1));
		}
	}

	void CLightEntity::setRange(float fRange)
	{
		if (0 != fRange)
		{
			mData.fRange = fRange * 0.5f;
			setScale(mData.fRange);
			if (nullptr != mpModelInstance)
			{
				mpModelInstance->setScale(mData.fRange);
			}
		}
	}

	float CLightEntity::getRange()
	{
		return mData.fRange;
	}


	void CLightEntity::setLightType(LightType type)
	{
		mLightType = type;
	}

	void CLightEntity::generateViewMT(const Vec3F& lookAt)
	{
		static Vec3F up(0, 1, 0);
		Math::buildMatrixLookAtLH(&mViewMatrix, &getPosition(), &lookAt, &up);
	}

	void CLightEntity::generateProjectMT(float fNear, float fFar, float fFov /*= (float)PI/4.0f*/, float fAspect /*= 1.0f*/)
	{
		Math::buildMatrixPerspectiveFovLH(&mProjectMatrix, fFov, fAspect, 1.0f, mData.fRange);
	}

	const Vec3F& CLightEntity::getDirection() const
	{
		return mData.positionOrDirection;
	}

	const Vec3F& CLightEntity::getColor() const
	{
		return mData.color;
	}

	LightType CLightEntity::getLightType() 
	{
		return mLightType;
	}

#ifdef _DEBUG
	void CLightEntity::setOriginPosition(const Vec3F& vec)
	{
		mOldPos = vec;
		mfSpeed = (rand() % 10) / 100.0f + 0.01f;
	}
#endif

	void CLightEntity::_tick(UINT32 dt)
	{
#ifdef _DEBUG
		mData.positionOrDirection.x = mOldPos.x + cos(mfValue) * 400;
		mData.positionOrDirection.z = mOldPos.z + sin(mfValue) * 400;
		mfValue += mfSpeed;
		setPosition(mData.positionOrDirection);
#endif
	}



}