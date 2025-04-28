#pragma once
#include <algorithm>
#include "ResourceFactory.h"
#include "Matrix.h"
#include "Vector.h"
#include "Material.h"
#include "Texture.h"
#include "Shader.h"
#include "Model.h"
#include "Foundation.h"
#include "Resource.h"
#include "LuaBind.h"
#include "Foundation.h"
#include "FoundationStruct.h"
#include "Instance.h"

namespace WL
{
	class CResourceMgr
	{
	public:
		CResourceMgr();
		virtual ~CResourceMgr();

		bool initialize();
		void unInitialize();
		void releaseResource();
		void removeResource(CResource* pResource);
		bool loadModel(CResource* pResourceBase, const std::string& strFile);
		bool loadMesh(CResource* pResourceBase, const std::string& strFile);
		bool loadMaterial(CResource* pResourceBase, const std::string& strFile);
		bool loadResource(CResource* pResourceBase, const std::string& strFile);
		bool loadTexture(CResource* pResourceBase, const std::string& strFile);
		bool loadShader(CResource* pResourceBase, const std::string& strFile);
		bool loadFont(CResource* pResourceBase, const std::string& strFile);
		bool loadGui(CResource* pResourceBase, const std::string& strFile);
		bool loadAnimation(CResource* pResourceBase, const std::string& strFile);

		CResource* createResource(ResourceType type)noexcept;
		CResource* createResource(std::string fileName, ResourceType type)noexcept;
		CResource* findResource(std::string fileName);
		CActorEntity* createBillBoardEntity(const std::string& szName, int nWidth, int nHeight, float zDepth = 1.0f);
		CModel* createBillBoardModel(const std::string& szName, int nWidth, int nHeight, float zDepth = 1.0f);
		CMesh* createBillBoardMesh(int nWidth, int nHeight, float zDepth = 1.0f);
		CMesh* createMesh(void* pVertices, int nVerticesCount, int VertexSize, void* pIndices, int nIndicesCount, int IndexSize);

		template<typename T>
		T* createResource(ResourceType type)noexcept
		{
			try
			{
				CResource* pResource = mpFactory->createResource(type);
				T* p = dynamic_cast<T*>(pResource);
				if (p != nullptr)
				{
					UINT32 uuid = pResource->getUUID();
					mResourceMap[uuid] = pResource;
					return p;
				}
				else
				{
					throw(0);
				}
			}
			catch (...)
			{
				return nullptr;
			}
		}
	private:
		void _addVSConstBuffer(CShader* pShader, int nKey);
		void _addHSConstBuffer(CShader* pShader, int nKey);
		void _addDSConstBuffer(CShader* pShader, int nKey);
		void _addPSConstBuffer(CShader* pShader, int nKey);
		void _addGSConstBuffer(CShader* pShader, int nKey);
		void _addCSConstBuffer(CShader* pShader, int nKey);
		void _addTextureConstBuffer(CShader* pShader, int nKey);
		void _addTextureBuffer(CShader* pShader, CMaterial* pMaterial, int nKey);
		void _addTypeBuffer(CShader* pShader, int nKey);
		void _addStructBuffer(CShader* pShader, int nKey);
		bool _loadMesh_obj(CMesh* pMesh, const std::string& strFile);
		bool _loadMesh_m(CMesh* pMesh, const std::string& strFile);
		bool _loadMesh_mTwo(CMesh* pMesh, const std::string& strFile);
		bool _loadMesh_fbx(CMesh* pMesh, const std::string& strFile);
		bool _loadMesh_txt(CMesh* pMesh, const std::string& strFile);
		bool _loadMesh_sdkmesh(CResource* pResourceBase, const std::string& strFile);
		std::vector<CMesh*> _loadMesh_omod(CResource* pMesh, const std::string& strFile);
		void calculationBoundBox(CMesh* pMesh);

	private:
		CResourceFactory* mpFactory = nullptr;
		std::map<UINT32, CResource*>		mResourceMap;
		std::map<std::string, CResource*>	mResourceMapByName;
		lua_State* mpLua = nullptr;
	};

}