#include "Model.h"
#include "WLEngine.h"
namespace WL
{
	CModel::CModel()
	{
		mName = "Model";
	}

	CModel::~CModel()
	{
		for (auto& item : mMeshsInfo)
		{
			WL_DECREASE(item.pMesh);
			WL_DECREASE(item.pMaterial);
			item.textures.clear();
		}
	}

	void CModel::addMesh(CMesh* pMesh)
	{
		if (nullptr != pMesh)
		{
			MeshNode meshNode;
			meshNode.pMaterial = mpMaterial;
			meshNode.pMesh = pMesh;
			mpMaterial->addRef();
			pMesh->addRef();
			addMesh(meshNode);
		}
	}

	void CModel::addMesh(const MeshNode& meshInfo)
	{
		mMeshsInfo.emplace_back(meshInfo);
		mAABB.unionBoundBox(meshInfo.pMesh->getBoundBox());

	}

	void CModel::addMesh(MeshNode* pMeshInfo)
	{
		addMesh(*pMeshInfo);
	}


	void CModel::addMesh(CMaterial* pMaterial, void* pVertices, int nVerticesCount, int VertexSize, void* pIndices, int nIndicesCount, int IndexSize,
		int nLod /* = 0*/,
		int vbMemoryType,/* = USAGE_DEFAULT*/
		int ibmemoryType/*= USAGE_IMMUTABLE*/)
	{
		auto pMesh = dynamic_cast<CMesh*>(GEngine->createResource(Mesh));
		if (nullptr != pMesh)
		{
			MeshNode meshNode;
			GEngine->commitCommand(commandType::StopDraw);
			pMesh->fillVertexBuffer(pVertices, nVerticesCount, VertexSize, nLod, vbMemoryType);
			pMesh->fillIndexBuffer(pIndices, nIndicesCount, IndexSize, nLod, ibmemoryType);
			GEngine->commitCommand(commandType::RecoverDraw);
			meshNode.pMaterial = pMaterial;
			meshNode.pMesh = pMesh;
			pMaterial->addRef();
			addMesh(meshNode);
		}
	}


	void CModel::addMesh(const std::string& szMaterial, void* pVertices, int nVerticesCount, int VertexSize, void* pIndices, int nIndicesCount, int IndexSize)
	{
		auto pMaterial = dynamic_cast<CMaterial*>(GEngine->createResource(szMaterial, Material));
		addMesh(pMaterial, pVertices, nVerticesCount, VertexSize, pIndices, nIndicesCount, IndexSize);
	}


	void CModel::addMesh(CMesh* pMesh, CMaterial* pMaterial)
	{
		if (nullptr != pMesh)
		{
			MeshNode meshNode;
			meshNode.pMaterial = pMaterial;
			meshNode.pMesh = pMesh;
			pMaterial->addRef();
			pMesh->addRef();
			addMesh(meshNode);
		} 
	}

	void CModel::removeMesh(const std::string& szName)
	{
		auto iter = mMeshsInfo.begin();
		auto iterEnd = mMeshsInfo.end();
		while (iter != iterEnd)
		{
			if (szName == (*iter).szName)
			{
				iter = mMeshsInfo.erase(iter);
				iterEnd = mMeshsInfo.end();
			}
			else
			{
				++iter;
			}
		}
	}


	void CModel::removeAllMesh()
	{
		for (auto& item : mMeshsInfo)
		{
			WL_DECREASE(item.pMaterial);
			WL_DECREASE(item.pMesh);
			SafeDelete(item.pAnimation);
			for (auto texture : item.textures)
			{
				WL_DECREASE(texture);
			}
			item.textures.clear();

		}
		mMeshsInfo.clear();
	}

	void CModel::addTexture(const std::string& szFlag, const std::string& szTexture)
	{
		if (szTexture.length() < 4)
		{
			return;
		}
		auto strList = Foundation::splitString<std::string>(szTexture, " ");
		int index = 0;
		if (strList.size() > 1)
		{
			index = Foundation::stringToType<int>(strList[1]);
		}
		if (strList.size() <= 2)
		{
			auto pTexture = dynamic_cast<CTexture*>(GEngine->createResource(strList[0], Texture));

			for (auto& item : mMeshsInfo)
			{
				auto pSample = item.pMaterial->getSampler(index);
				if (nullptr != pSample)
				{
					pTexture->setSampleState(item.pMaterial->getSampler(index));
				}
				else
				{
					pSample = item.pMaterial->getSampler(0);
					if (nullptr != pSample)
					{
						pTexture->setSampleState(pSample);
					}
				}
				if (0 == szFlag.compare("baseTexture"))
				{
					item.textures[0] = pTexture;
				}
				else if (0 == szFlag.compare("normalTexture"))
				{
					item.textures[1] = pTexture;
				}
				else if (0 == szFlag.compare("detailTexture"))
				{
					item.textures[2] = pTexture;
				}
				else if (0 == szFlag.compare("lightMapTexture"))
				{
					item.textures[3] = pTexture;
				}
			}
		}
		if (0 == szFlag.compare("textureArray"))
		{
			for (auto& item : mMeshsInfo)
			{
				auto pSample = item.pMaterial->getSampler(0);
				item.textureArray.setSamplerState(pSample);
				item.textureArray.addTexture(szTexture);
			}
		}
	}

	void CModel::setTextureArray(const CTextureArray* pTextureArray)
	{
		for (auto& item : mMeshsInfo)
		{
			item.textureArray = *pTextureArray;
		}
	}

	std::vector<MeshNode>& CModel::getAllMeshInfo()
	{
		return mMeshsInfo;
	}

	MeshNode* CModel::getMeshInfo(const std::string& szName)
	{
		MeshNode* pInfo = nullptr;
		auto iter = mMeshsInfo.begin();
		auto iterEnd = mMeshsInfo.end();
		while (iter != iterEnd)
		{
			if (szName == (*iter).szName)
			{
				pInfo = &(*iter);
			}
		}
		return pInfo;
	}


	void CModel::setMaterial(CMaterial* pMaterial)
	{
		if (mpMaterial != pMaterial)
		{
			WL_DECREASE(mpMaterial);
			mpMaterial = pMaterial;
			WL_INCREASE(mpMaterial);
		}
	}


	void CModel::setMaterial(const std::string& szMaterial)
	{
		auto pMaterial = dynamic_cast<CMaterial*>(GEngine->createResource(szMaterial, Material));
		setMaterial(pMaterial);
	}

	CMaterial* CModel::getMaterial() const
	{
		mpMaterial->addRef();
		return mpMaterial;
	}

}