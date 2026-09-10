#pragma once
#include "Component.h"
#include "Vector.h"
#include "Matrix.h"
#include "MathLib.h"
#include "AABBox.h"
#include "BoundingSphere.h"

namespace WL
{
	class WLENGINE_API CTranformComponet : public CComponent
	{
	public:
		CTranformComponet();
		virtual ~CTranformComponet();
		
		void setPosition(const Vec3F& vec);
		const Vec3F& getPosition()const;

		void setScale(float fScale);
		void setScale(const Vec3F& vec);
		const Vec3F& getScale()const;


		void setRotation(const Vec3F& vec);
		const Vec3F& getRotation()const;

		SMatrix44& getMT();

		SMatrix44& getTransposeMT();

		const CAABBox& getBoundBox() const;
		
		const CBoundingSphere& getBoundingSphere() const;

		void updateMatrix();

	protected:
		CAABBox	mAABB;
		CBoundingSphere	mSphere;

	private:
		Vec3F	mPosition = { 0, 0, 0 };
		Vec3F	mScale = { 1, 1, 1 };
		Vec3F	mRotation = { 0, 0, 0 };
		SMatrix44 mRT;
		SMatrix44 mTransposeRT;
		SMatrix44 mPositonRT;
		SMatrix44 mScaleRT;
		SMatrix44 mRotationRT;

		bool	mbUpdate = false;
	};
}