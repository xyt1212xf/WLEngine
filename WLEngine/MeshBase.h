#pragma once
#include "Resource.h"
#include "GPUBuffer.h"
#include "FoundationStruct.h"
#include "Transform.h"
#include "VertexFormat.h"

namespace WL
{
	class CMeshBase : public CResource,
					  public CTranformComponet
	{
	public:
		CMeshBase();
		virtual ~CMeshBase();
		CGpuBuffer* getVertexBuffer()const;
		CGpuBuffer* getIndexBuffer()const;
		UINT32 getVertexTypeSize(int nLod = -1)const;
		UINT32 getIndexCount(int nLod = -1)const;
		UINT32 getVerticesCount(int nLod = -1)const;
		UINT32 getIndeiceType();
		UINT32 getVertexOffset();
		byte* getDataStream()const;

		Vec3F* getVertices(int nIndex);
		Vec3F* getNormals(int nIndex);
		Vec3F* getBinormals(int nIndex);
		Vec3F* getTanget(int nIndex);
		Vec2F* getUVs(int nIndex);
		Vec4I* getBoneIndex(int nIndex);
		Vec3F* getWeight(int nIndex);
		Vec4F* getColorFloat(int nIndex);
		UINT32* getColorUINT(int nIndex);


		const VertexFormat* getVertexFormat() const;
		inline const Vec3F& getVertex(int nIndex)const
		{
			INT32 nOffset = nIndex * mVertexTypeSize[mLodLevel];
			if (nOffset < mpDataLength)
			{
				return *(Vec3F*)(mpDataStream + nOffset);
			}
			else
			{
				static Vec3F vDefault = { 0, 0, 0 };
				return vDefault;
			}
		}

		const std::vector<UINT32>& getIndices() const;

	protected:
		VertexFormat						mVertexFormat;
		std::vector<VertexVT>				mVerticesPT;
		std::vector<VertexFormatVTNTB>		mVertices;
		std::vector<VertexFormatVTNT>		mVerticesT;
		std::vector<VertexFormatVTN>		mVerticesVTN;
		std::vector<VertexFormatVTNTBSkin>	mVerticesSkin;
		bool mbFreeDataStream = false;
		byte* mpDataStream = nullptr;
		INT32 mpDataLength = 0;
		int mIndeiceType = 0;
		int mLodLevel = 0;
		int mVertexOffset = 0;
		std::vector<std::vector<UINT32>> mIndices;
		std::vector<UINT32>		mVertexTypeSize;
		std::vector<CGpuBuffer*> mpLodVB;
		std::vector<CGpuBuffer*> mpLodIB;
		std::vector<UINT32> mVertexCount;
		std::vector<UINT32> mIndexCount;
	};
}