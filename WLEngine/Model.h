
#pragma once
#include "Mesh.h"
#include "Vector.h"
#include "Resource.h"
#include "Transform.h"
#include "Material.h"
#include "RenderChunk.h"
#include "ComponentGroup.h"

namespace WL
{
	struct SMeshNode
	{
		SMeshNode()
		{
			textures.resize(8);
		}
		SMeshNode(const SMeshNode& meshNode) noexcept
		{
			szName = meshNode.szName;
			pMesh = meshNode.pMesh;
			pMaterial = meshNode.pMaterial;
			pAnimation = meshNode.pAnimation;
		//	geometry = meshNode.geometry;
			textures.insert(textures.begin(), meshNode.textures.begin(), meshNode.textures.end());
			for( auto item : textures )
			{
				if (nullptr != item)
				{
					item->addRef();
				}
			}
			textureArray = meshNode.textureArray;
		}

		SMeshNode(SMeshNode&& meshNode) noexcept
		{
			szName = meshNode.szName;
			pMesh = meshNode.pMesh;
			pMaterial = meshNode.pMaterial;
			pAnimation = meshNode.pAnimation;
		//	geometry = meshNode.geometry;
			textures.insert(textures.begin(), meshNode.textures.begin(), meshNode.textures.end());
			meshNode.textures.clear();
			textureArray = meshNode.textureArray;
		}
		SMeshNode& operator = (SMeshNode&& meshNode) noexcept
		{ 
			// move assign
			szName = meshNode.szName;
			pMesh = meshNode.pMesh;
			pMaterial = meshNode.pMaterial;
			pAnimation = meshNode.pAnimation;
		//	geometry = meshNode.geometry;
			textures.insert(textures.begin(), meshNode.textures.begin(), meshNode.textures.end());
			meshNode.textures.clear();
			textureArray = meshNode.textureArray;
			return *this;
		}

		SMeshNode& operator=(const SMeshNode& meshNode)
		{ // copy assign
			szName = meshNode.szName;
			pMesh = meshNode.pMesh;
			pMaterial = meshNode.pMaterial;
			pAnimation = meshNode.pAnimation;
		//	geometry = meshNode.geometry;
			textures.insert(textures.begin(), meshNode.textures.begin(), meshNode.textures.end());
			textureArray = meshNode.textureArray;
			for (auto item : textures)
			{
				if (nullptr != item)
				{
					item->addRef();
				}
			}
			return *this;
		}

		~SMeshNode()
		{
			for (auto item : textures)
			{
				WL_DECREASE(item);
			}
		}
		
		std::string szName = "";
		CMesh* pMesh = nullptr;
		CMaterial* pMaterial = nullptr;
		CSkeletonAniComponent* pAnimation = nullptr;
		std::vector<CTexture*> textures;
		CTextureArray textureArray;
	};

	class CModel : public CResource,
				   public CTranformComponet,
				   public SComponentGrounp
	{
		friend class CModelInstance;
	public:
		CModel();
		virtual ~CModel();
		void addMesh(CMesh* pMesh, CMaterial* pMaterial);
		void addMesh(SMeshNode* pMeshInfo);
		void addMesh(const SMeshNode& meshInfo);
		void addMesh(CMesh* pMesh);
		void addMesh(CMaterial* pMaterial, void* pVertices, int nVerticesCount, int VertexSize, void* pIndices, int nIndicesCount, int IndexSize,
			int nLod = 0,
			int vbMemoryType = USAGE_DEFAULT,
			int ibmemoryType = USAGE_IMMUTABLE);
		void addMesh(const std::string& szMaterial, void* pVertices, int nVerticesCount, int VertexSize, void* pIndices, int nIndicesCount, int IndexSize);
		void removeMesh(const std::string& szName);
		void removeAllMesh();
		void addTexture(const std::string& szFlag, const std::string& szTexture);
		void setTextureArray(const CTextureArray* pTextureArray);

		std::vector<SMeshNode>& getAllMeshInfo();
		SMeshNode* getMeshInfo(const std::string& szName);
		void setMaterial(const std::string& szMaterial);
		void setMaterial(CMaterial* pMaterial);
		CMaterial* getMaterial() const;

	private:
		std::vector<SMeshNode>	mMeshsInfo;
		CMaterial* mpMaterial = nullptr;
	};

}