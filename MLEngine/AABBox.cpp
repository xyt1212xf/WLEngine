#include "AABBox.h"
#include "MathLib.h"
#include "Foundation.h"
#include "ModelInstance.h"

namespace WL
{
	CAABBox::CAABBox()
		: CAABBox(Vec3F(), Vec3F())
	{
	}


	CAABBox::CAABBox(Vec3F min, Vec3F max)
	{
		mbDirty = true;
		mPt = Vec3F(0, 0, 0);
		mExtent = Vec3F(0, 0, 0);
		ZeroMemory(mBoundBoxPit, sizeof(mBoundBoxPit));
	}

	CAABBox::~CAABBox()
	{
		WL_DECREASE(mpModelInstance);
	}

	void CAABBox::setPosition(const Vec3F& pt)
	{
		mbDirty = true;
		mPt = pt;

		modifyAABB();
		//update();
	}

	void CAABBox::setScale(const Vec3F& scale)
	{
		mbDirty = true;
		mScale = scale;
	
		modifyAABB();
		//update();
	}

	void CAABBox::setRotation(const Vec3F& rotation)
	{
		mbDirty = true;
	}

	void CAABBox::setExtent(const Vec3F& radiuse)
	{
		mbDirty = true;
		mExtent = radiuse;
		//update();
	}

	void CAABBox::setRadius(float radius)
	{
		mRadius = radius;
	}

	const Vec3F& CAABBox::getCenter() const
	{
		return mPt;
	}

	const Vec3F& CAABBox::getExtent()const
	{
		return mExtent;
	}

	float CAABBox::getRadius() const
	{
		return mRadius;
	}

	const Vec3F& CAABBox::getMinPos() const
	{
		return mMinPos;
	}

	const Vec3F& CAABBox::getMaxPos() const
	{
		return mMaxPos;
	}

	const Vec3F& CAABBox::size() const
	{
		return mSize;
	}

	const Vec3F& CAABBox::getOriginalMinPos() const
	{
		return mOriginalMinPos;
	}

	const Vec3F& CAABBox::getOriginalMaxPos() const
	{
		return mOriginalMaxPos;
	}

	void CAABBox::setVisible(bool bValue)
	{
		bValue ? show() : hide();
	}

	void CAABBox::show()
	{
		mVisible = true;
	}

	void CAABBox::hide()
	{
		mVisible = false;
	}

	bool CAABBox::isShow()
	{
		return mVisible;
	}

	bool CAABBox::checkPoint(const Vec3F& p) const
	{
		if (p.x < mMinPos.x || p.x > mMaxPos.x)
		{
			return false;
		}
		if (p.y < mMinPos.y || p.y > mMaxPos.y)
		{
			return false;
		}
		if (p.z < mMinPos.z || p.z > mMaxPos.z)
		{
			return false;
		}
		return true;
	}

	bool CAABBox::checkBoundBox(const CAABBox& boundbox) const
	{
		if (mMinPos.x > boundbox.mMaxPos.x)
		{
			return false;
		}
		if (mMaxPos.x < boundbox.mMinPos.x)
		{
			return false;
		}

		if (mMinPos.y > boundbox.mMaxPos.y)
		{
			return false;
		}
		if (mMaxPos.y < boundbox.mMinPos.y)
		{
			return false;
		}

		if (mMinPos.z > boundbox.mMaxPos.z)
		{
			return false;
		}
		if (mMaxPos.z < boundbox.mMinPos.z)
		{
			return false;
		}
		return true;
	}



	bool CAABBox::checkBoundBox(const CAABBox& boundbox, CAABBox* pBoundBox)const
	{
		if (checkBoundBox(boundbox))
		{
			if (pBoundBox != nullptr)
			{
				pBoundBox->mMinPos.x = max(mMinPos.x, boundbox.mMinPos.x);
				pBoundBox->mMaxPos.x = min(mMaxPos.x, boundbox.mMaxPos.x);
				pBoundBox->mMinPos.y = max(mMinPos.y, boundbox.mMinPos.y);
				pBoundBox->mMaxPos.y = min(mMaxPos.y, boundbox.mMaxPos.y);
				pBoundBox->mMinPos.z = max(mMinPos.z, boundbox.mMinPos.z);
				pBoundBox->mMaxPos.z = min(mMaxPos.z, boundbox.mMaxPos.z);

			}
			return true;
		}
		return false;
	}

	bool CAABBox::insideBoundBox(const CAABBox& bigBoundbox)
	{
		if ( mMinPos.x < bigBoundbox.mMinPos.x)
		{
			return false;
		}
		if (mMinPos.y < bigBoundbox.mMinPos.y)
		{
			return false;
		}
		if (mMinPos.z < bigBoundbox.mMinPos.z)
		{
			return false;
		}
		if (mMaxPos.x > bigBoundbox.mMaxPos.x)
		{
			return false;
		}
		if (mMaxPos.y > bigBoundbox.mMaxPos.y)
		{
			return false;
		}
		if (mMaxPos.z > bigBoundbox.mMaxPos.z)
		{
			return false;
		}
		return true;
	}

	void CAABBox::reset()
	{
		mPt = Vec3F(0, 0, 0);
		mMinPos = mPt;
		mMaxPos = mPt;
		mExtent = mPt;
		mOriginalPt = mPt;
		mOriginalMinPos = mPt;
		mOriginalMaxPos = mPt;
		mOriginalRadius = mPt;
		for (int i = 0; i < 8; ++i)
		{
			mBoundBoxPit[i] = mPt;
			mOriginalBoundBoxPit[i].x = mPt.x;
			mOriginalBoundBoxPit[i].y = mPt.y;
			mOriginalBoundBoxPit[i].z = mPt.z;
		}
	}


	void CAABBox::setBoundBoxRange(const Vec3F& min, const Vec3F& max)
	{
		mOriginalMinPos = min;
		mOriginalMaxPos = max;
		mMinPos = min;
		mMaxPos = max;

		mSize.x = fabs(max.x - min.x);
		mSize.y = fabs(max.y - min.y);
		mSize.z = fabs(max.z - min.z);

		mOriginalPt = (min + max);
		mOriginalPt *= 0.5f;
		mOriginalRadius = (max - min);
		mOriginalRadius *= 0.5f;
		mExtent = mOriginalRadius;
		mPt = mOriginalPt;
		//mRadiusLength = max.length();
		Vec3F minPt = mPt - mExtent;
		Vec3F maxPt = mPt + mExtent;
		mBoundBoxPit[0].x = min.x;
		mBoundBoxPit[0].y = min.y;
		mBoundBoxPit[0].z = max.z;

		mBoundBoxPit[1].x = max.x;
		mBoundBoxPit[1].y = min.y;
		mBoundBoxPit[1].z = max.z;

		mBoundBoxPit[2].x = max.x;
		mBoundBoxPit[2].y = min.y;
		mBoundBoxPit[2].z = min.z;

		mBoundBoxPit[3].x = min.x;
		mBoundBoxPit[3].y = min.y;
		mBoundBoxPit[3].z = min.z;

		mBoundBoxPit[4].x = min.x;
		mBoundBoxPit[4].y = max.y;
		mBoundBoxPit[4].z = max.z;

		mBoundBoxPit[5].x = max.x;
		mBoundBoxPit[5].y = max.y;
		mBoundBoxPit[5].z = max.z;

		mBoundBoxPit[6].x = max.x;
		mBoundBoxPit[6].y = max.y;
		mBoundBoxPit[6].z = min.z;

		mBoundBoxPit[7].x = min.x;
		mBoundBoxPit[7].y = max.y;
		mBoundBoxPit[7].z = min.z;

		memcpy(mOriginalBoundBoxPit, mBoundBoxPit, sizeof(mBoundBoxPit));
	}

	void CAABBox::setBoundBoxRange(float fValue)
	{
		mMinPos -= fValue;
		mMaxPos += fValue;
		setBoundBoxRange(mMinPos, mMaxPos);
	}

	void CAABBox::unionBoundBox(const CAABBox& aabb)
	{
		Vec3F fMin;
		Vec3F fMax;

		//////////////////////////////////////////////////////////////////////////
		if (aabb.mMinPos.x <= mMinPos.x)
		{
			fMin.x = aabb.mMinPos.x;
		}
		else
		{
			fMin.x = mMinPos.x;
		}

		if (aabb.mMinPos.y <= mMinPos.y)
		{
			fMin.y = aabb.mMinPos.y;
		}
		else
		{
			fMin.y = mOriginalMinPos.y;
		}

		if (aabb.mMinPos.z <= mMinPos.z)
		{
			fMin.z = aabb.mMinPos.z;
		}
		else
		{
			fMin.z = mMinPos.z;
		}

		//////////////////////////////////////////////////////////////////////////
		if (aabb.mMaxPos.x >= mMaxPos.x)
		{
			fMax.x = aabb.mMaxPos.x;
		}
		else
		{
			fMax.x = mMaxPos.x;
		}

		if (aabb.mMaxPos.y >= mMaxPos.y)
		{
			fMax.y = aabb.mMaxPos.y;
		}
		else
		{
			fMax.y = mMaxPos.y;
		}

		if (aabb.mMaxPos.z >= mMaxPos.z)
		{
			fMax.z = aabb.mMaxPos.z;
		}
		else
		{
			fMax.z = mMaxPos.z;
		}
		mOriginalMinPos = fMin;
		mOriginalMaxPos = fMax;
		setBoundBoxRange(fMin, fMax);
	}

	void CAABBox::buildLineMesh()
	{
#if 0
		//
		VertexFormat vFormat;
		vFormat.addElement(V_POSITION);
		vFormat.addElement(V_TEXTURE);
		vFormat.addElement(V_NORMAL);
		int nSize = vFormat.getVertexSize();
		std::vector<UINT8> vertices(nSize * 8);
		std::vector<UINT16> indices;
		UINT8* pData = vertices.data();
		for (int i = 0; i < CountArray(mBoundBoxPit); ++i)
		{
			Vec3F* pVerticsData = (Vec3F*)(pData);
			pVerticsData->x = mBoundBoxPit[i].x;
			pVerticsData->y = mBoundBoxPit[i].y;
			pVerticsData->z = mBoundBoxPit[i].z;
			pData += nSize;
		}
		for( int i= 0; i < 3; ++i)
		{
			indices.push_back(i + 0);
			indices.push_back(i + 1);

			indices.push_back(i + 4);
			indices.push_back(i + 5);

			indices.push_back(i + 0);
			indices.push_back(i + 4);
		}
		indices.push_back(0);
		indices.push_back(3);

		indices.push_back(7);
		indices.push_back(4);

		indices.push_back(3);
		indices.push_back(7);
		SafeRelease(mpModelInstance);
		mpModelInstance = Foundation::generateModel(vertices.data(), 8, nSize, indices.data(), indices.size(), sizeof(UINT16),
														"resource/Material/newVoxel.ma", 
														"resource/Texture/grass_top.png");
		mpModelInstance->getMaterialInstance()->setTopology(PRIMITIVE_TOPOLOGY_LINELIST);
#endif
	}

	bool CAABBox::operator==(const CAABBox& box)
	{
		if (!memcmp(&mMinPos.x, &box.mMinPos.x, sizeof(mMinPos)) ||
			!memcmp(&mMaxPos.x, &box.mMaxPos.x, sizeof(mMaxPos)))
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	void CAABBox::modifyAABB()
	{
		for (int i = 0; i < CountArray(mBoundBoxPit); ++i)
		{
			mBoundBoxPit[i].x = mOriginalBoundBoxPit[i].x * mScale.x + mPt.x;
			mBoundBoxPit[i].y = mOriginalBoundBoxPit[i].y * mScale.y + mPt.y;
			mBoundBoxPit[i].z = mOriginalBoundBoxPit[i].z * mScale.z + mPt.z;
		}
		mMinPos = mOriginalMinPos * mScale + mPt;
		mMaxPos = mOriginalMaxPos * mScale + mPt;
		if (nullptr != mpModelInstance)
		{
			buildLineMesh();
		}
	}

}
