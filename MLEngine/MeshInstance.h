#pragma once
#include "Instance.h"
#include "Transform.h"
#include "Mesh.h"
#include "SkeletonAniComponent.h"
#include "MaterialInstance.h"

namespace WL
{
	class CMeshInstance : public CInstance,
						  public CTranformComponet
	{
	public:
		CMeshInstance(CMesh* pMesh);
		virtual ~CMeshInstance();
		void bindMesh(CMesh* pMesh);
		void operator = (const CMeshInstance& instance);
		CGpuBuffer* getVertexBuffer()const;
		CGpuBuffer* getIndexBuffer()const;
		UINT32 getVertexTypeSize(int nLod = -1) const;

		UINT32 getIndeiceType();
		UINT32 getVertexOffset();
		CMesh* getMesh() const;

		void setLocalPosition(const Vec3F& position);
		void setLocalScale(const Vec3F& scale);
		void setLocalRotation(const Vec3F& rotation);
		const Vec3F& getLocalPosition() const;
		const Vec3F& getLocalScale() const;
		const Vec3F& getLocalRotation() const;

		virtual void setPosition(const Vec3F& vec)final;
		virtual void setScale(const Vec3F& vec)final;
		virtual void setRotation(const Vec3F& vec) final;

		void setViewMT(SMatrix44* pMT);
		void setTransposeViewMT(SMatrix44* pMT);

		void setProjectMT(SMatrix44* pMT);
		void setTransposeProjectMT(SMatrix44* pMT);

		SMatrix44& getViewMT()const;
		SMatrix44& getTransposeViewMT()const;

		SMatrix44& getProjectMT()const;
		SMatrix44& getTransposeProjectMT()const;
		SMatrix44& getTransposeViweProjectMatrix()const;

		void setIndexCount(UINT32 nCount);
		UINT32 getIndexCount(int nLod = -1)const;
		void setStartIndexLocation(UINT32 nStart);
		UINT32 getStartIndexLocation() const;
		void setBaseVertexLocation(UINT32 nStart);
		UINT32 getBaseVertexLocation() const;

	protected:
		CMesh* mpMesh = nullptr;
		Vec3F	mLocalPosition = {0, 0, 0};
		Vec3F	mLocalScale = { 1, 1, 1 };
		Vec3F	mLocalRotation = { 0, 0, 0 };
		SMatrix44* mpViewMT = nullptr;
		SMatrix44* mpTransposeViewMT = nullptr;
		SMatrix44* mpProjectMT = nullptr;
		SMatrix44* mpTransposeProjectMT = nullptr;

		mutable SMatrix44 mTransposeViewProjectMT;
		UINT32 mStartIndex = 0;
		UINT32 mBaseStartIndex = 0;
		UINT32 mIndexCount = 0;
	};

	struct SMeshInstanceInfo 
	{
		std::string name = "";
		CMeshInstance* pMeshInstance = nullptr;
		CMaterialInstance* pMaterialInstance = nullptr;
		CSkeletonAniComponent* pAnimation = nullptr;
		void operator = (const SMeshInstanceInfo& mat);
	};
}