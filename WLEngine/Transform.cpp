#include "Transform.h"
namespace WL
{
	CTranformComponet::CTranformComponet()
	{
		mComponentName = "TranformComponet";
		Math::matrixIdentity(&mPositonRT);
		Math::matrixIdentity(&mScaleRT);
		Math::matrixIdentity(&mRotationRT);
		mScaleRT.m[0][0] = mScale.x;
		mScaleRT.m[1][1] = mScale.y;
		mScaleRT.m[2][2] = mScale.z;
	}

	CTranformComponet::~CTranformComponet()
	{

	}

	void CTranformComponet::setPosition(const Vec3F& vec)
	{
		mPosition = vec;
		Math::matrixTranslation(&mPositonRT, mPosition.x, mPosition.y, mPosition.z);
		mbUpdate = true;
		mAABB.setPosition(vec);
	}

	const Vec3F& CTranformComponet::getPosition() const
	{
		return mPosition;
	}

	void CTranformComponet::setScale(const Vec3F& vec)
	{
		mScale = vec;
		Math::matrixScaling(&mScaleRT, mScale.x, mScale.y, mScale.z);
		mbUpdate = true;
		mAABB.setScale(vec);
	}

	void CTranformComponet::setScale(float fScale)
	{
		setScale(Vec3F(fScale, fScale, fScale));
	}

	const Vec3F& CTranformComponet::getScale() const
	{
		return mScale;
	}

	void CTranformComponet::setRotation(const Vec3F& vec)
	{
		mRotation = vec;
		Math::matrixRotationEulerian(&mRotationRT, vec);
		mbUpdate = true;
		mAABB.setRotation(vec);
	}

	const Vec3F& CTranformComponet::getRotation() const
	{
		return mRotation;
	}

	SMatrix44& CTranformComponet::getMT() 
	{
		return mRT;
	}

	SMatrix44& CTranformComponet::getTransposeMT()
	{
		return mTransposeRT;
	}

	const CAABBox& CTranformComponet::getBoundBox()const
	{
		return mAABB;
	}

	const CBoundingSphere& CTranformComponet::getBoundingSphere() const
	{
		return mSphere;
	}

	void CTranformComponet::updateMatrix()
	{
		if (mbUpdate)
		{
			mbUpdate = false;
			mRT = mRotationRT * mScaleRT * mPositonRT;
			Math::matrixTranspose(&mTransposeRT, &mRT);
		}
	}

}