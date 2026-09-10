#pragma once
#include "Vector.h"
#include "Shader.h"

namespace WL 
{
	class CModelInstance;
	class CAABBox
	{
	public:
		CAABBox();
		CAABBox(Vec3F min, Vec3F max);	
		~CAABBox();
		void setPosition(const Vec3F& pt);
		void setScale(const Vec3F& scale);

		void setRotation(const Vec3F& rotation);
		void setExtent(const Vec3F& radius);
		void setRadius(float radius);


		const Vec3F& getCenter()const;
		const Vec3F& getExtent()const;
		float getRadius()const;
		const Vec3F& getMinPos()const;
		const Vec3F& getMaxPos()const;
		const Vec3F& size()const;
		const Vec3F& getOriginalMinPos()const;
		const Vec3F& getOriginalMaxPos()const;

		void setVisible(bool bValue);
		void show();
		void hide();
		bool isShow();
		bool checkPoint(const Vec3F& p)const;
		bool checkBoundBox(const CAABBox& boundbox)const;
		bool checkBoundBox(const CAABBox& boundbox, CAABBox* pBoundBox)const;
		bool insideBoundBox(const CAABBox& bigBoundbox);

		void reset();
		void setBoundBoxRange(const Vec3F& fMin, const Vec3F& fMax);
		void setBoundBoxRange(float fValue);
		bool operator == (const CAABBox& box);
		void unionBoundBox(const CAABBox& aabb);
		void buildLineMesh();
		
	private:
		void modifyAABB();

	public:
		Vec3F mMinPos = { 0,0,0 };
		Vec3F mMaxPos = { 0,0,0 };
		Vec3F mPt = { 0,0,0 };
		Vec3F mScale = { 1,1,1 };
		Vec3F mExtent = { 0,0,0 };
		Vec3F mBoundBoxPit[8];
		Vec3F mSize;
		Vec3F mOriginalMinPos;
		Vec3F mOriginalMaxPos;
		Vec3F mOriginalPt;
		Vec3F mOriginalRadius;
		Vec3F mOriginalBoundBoxPit[8];

		float	mRadius = 0;
		bool mbDirty =false;
		bool mVisible = false;
		CModelInstance* mpModelInstance = nullptr;
	};
}
