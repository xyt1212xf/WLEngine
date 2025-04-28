#include "MeshBase.h"
#define LODLEVEL_MAX 3
namespace WL
{
	CMeshBase::CMeshBase()
	{
		mpLodVB.resize(LODLEVEL_MAX);
		mpLodIB.resize(LODLEVEL_MAX);
		mVertexCount.resize(LODLEVEL_MAX);
		mIndexCount.resize(LODLEVEL_MAX);
		mVertexTypeSize.resize(LODLEVEL_MAX);
		mIndices.resize(LODLEVEL_MAX);
	}

	CMeshBase::~CMeshBase()
	{
		for( auto item : mpLodVB )
		{
			WL_DELETE(item, GpuBuffer);
		}
		for (auto item : mpLodIB)
		{
			WL_DELETE(item, GpuBuffer);
		}
		mpLodVB.clear();
		mpLodIB.clear();
		if (mbFreeDataStream)
		{
			WL_DELETE_ARRAY(mpDataStream);
		}
	}

	CGpuBuffer* CMeshBase::getVertexBuffer() const
	{
		return mpLodVB[mLodLevel];
	}

	CGpuBuffer* CMeshBase::getIndexBuffer() const
	{
		return mpLodIB[mLodLevel];
	}

	UINT32 CMeshBase::getVertexTypeSize(int nLod)const
	{
		if (-1 != nLod)
		{
			return mVertexTypeSize[nLod];
		}
		else
		{
			return mVertexTypeSize[mLodLevel];
		}
	}

	UINT32 CMeshBase::getIndexCount(int nLod /*= -1*/) const
	{
		if (-1 != nLod)
		{
			return mIndexCount[nLod];
		}
		else
		{
			return mIndexCount[mLodLevel];
		}
	}

	UINT32 CMeshBase::getVerticesCount(int nLod /*= -1*/) const
	{
		if (-1 != nLod)
		{
			return mVertexCount[nLod];
		}
		else
		{
			return mVertexCount[mLodLevel];
		}
	}

	//inline const Vec3F& CMeshBase::getVertex(int nIndex) const
	//{
	//	int nOffset = nIndex* mVertexTypeSize[mLodLevel];
	//	if (nOffset < mpDataLength)
	//	{
	//		return *(Vec3F*)mpDataStream[nOffset];
	//	}
	//	else
	//	{
	//		static Vec3F vDefault = {0, 0, 0};
	//		return vDefault;
	//	}
	//}

	const std::vector<UINT32>& CMeshBase::getIndices() const
	{
		return mIndices[mLodLevel];
	}

	UINT32 CMeshBase::getIndeiceType()
	{
		return mIndeiceType;
	}

	UINT32 CMeshBase::getVertexOffset()
	{
		return mVertexOffset;
	}

	byte* CMeshBase::getDataStream() const
	{
		return mpDataStream;
	}

	Vec3F* CMeshBase::getVertices(int nIndex)
	{
		if (mpDataLength < mVertexFormat.getVertexSize() * nIndex)
		{
			int offset = mVertexFormat.getVerticesOffset();
			return (Vec3F*)(mpDataStream + offset);
		}
		else
		{
			return nullptr;
		}
	}

	Vec3F* CMeshBase::getNormals(int nIndex)
	{
		if (mpDataLength < mVertexFormat.getVertexSize() * nIndex)
		{
			int offset = mVertexFormat.getNormalsOffset();
			return (Vec3F*)(mpDataStream + offset);
		}
		else
		{
			return nullptr;
		}
	}

	Vec3F* CMeshBase::getBinormals(int nIndex)
	{
		if (mpDataLength < mVertexFormat.getVertexSize() * nIndex)
		{
			int offset = mVertexFormat.getBinormalsOffset();
			return (Vec3F*)(mpDataStream + offset);
		}
		else
		{
			return nullptr;
		}
	}

	Vec3F* CMeshBase::getTanget(int nIndex)
	{
		if (mpDataLength < mVertexFormat.getVertexSize() * nIndex)
		{
			int offset = mVertexFormat.getTangetOffset();
			return (Vec3F*)(mpDataStream + offset);
		}
		else
		{
			return nullptr;
		}
	}

	Vec2F* CMeshBase::getUVs(int nIndex)
	{
		if (mpDataLength < mVertexFormat.getVertexSize() * nIndex)
		{
			int offset = mVertexFormat.getUVsOffset();
			return (Vec2F*)(mpDataStream + offset);
		}
		else
		{
			return nullptr;
		}
	}

	Vec4I* CMeshBase::getBoneIndex(int nIndex)
	{
		if (mpDataLength < mVertexFormat.getVertexSize() * nIndex)
		{
			int offset = mVertexFormat.getBoneIndexOffset();
			return (Vec4I*)(mpDataStream + offset);
		}
		else
		{
			return nullptr;
		}
	}

	Vec3F* CMeshBase::getWeight(int nIndex)
	{
		if (mpDataLength < mVertexFormat.getVertexSize() * nIndex)
		{
			int offset = mVertexFormat.getWeightOffset();
			return (Vec3F*)(mpDataStream + offset);
		}
		else
		{
			return nullptr;
		}
	}

	Vec4F* CMeshBase::getColorFloat(int nIndex)
	{
		if (mpDataLength < mVertexFormat.getVertexSize() * nIndex)
		{
			int offset = mVertexFormat.getColorFloatOffset();
			return (Vec4F*)(mpDataStream + offset);
		}
		else
		{
			return nullptr;
		}
	}

	UINT32* CMeshBase::getColorUINT(int nIndex)
	{
		if (mpDataLength < mVertexFormat.getVertexSize() * nIndex)
		{
			int offset = mVertexFormat.getColorUINTOffset();
			return (UINT32*)(mpDataStream + offset);
		}
		else
		{
			return nullptr;
		}
	}

	const VertexFormat* CMeshBase::getVertexFormat() const
	{
		return &mVertexFormat;
	}

}