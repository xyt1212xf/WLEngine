
#pragma once
#include "Mesh.h"
#include "Vector.h"
#include "Resource.h"
#include "Transform.h"
#include "Material.h"
#include "RenderChunk.h"


namespace WL
{
	struct MeshNode
	{
		MeshNode()
		{
			textures.resize(8);
		}
		MeshNode(const MeshNode& meshNode) noexcept
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

		MeshNode(MeshNode&& meshNode) noexcept
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
		MeshNode& operator = (MeshNode&& meshNode) noexcept
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

		MeshNode& operator=(const MeshNode& meshNode)
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

		~MeshNode()
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
				   public ComponentGrounp
	{
		friend class CModelInstance;
	public:
		CModel();
		virtual ~CModel();
		void addMesh(CMesh* pMesh, CMaterial* pMaterial);
		void addMesh(MeshNode* pMeshInfo);
		void addMesh(const MeshNode& meshInfo);
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

		std::vector<MeshNode>& getAllMeshInfo();
		MeshNode* getMeshInfo(const std::string& szName);
		void setMaterial(const std::string& szMaterial);
		void setMaterial(CMaterial* pMaterial);
		CMaterial* getMaterial() const;

	private:
		std::vector<MeshNode>	mMeshsInfo;
		CMaterial* mpMaterial = nullptr;
	};

}