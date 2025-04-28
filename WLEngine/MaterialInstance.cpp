#include "MaterialInstance.h"
#include "WLEngine.h"


namespace WL
{
	CMaterialInstance::CMaterialInstance(CMaterial* pMaterial)
	{
		bindMaterial(pMaterial);
	}

	CMaterialInstance::~CMaterialInstance()
	{
		WL_DECREASE(mpMaterial);
		for( auto item : mArrayShaderIns)
		{
			WL_DELETE(item, Instance);
		}
		mArrayShaderIns.clear();
	}

	CInstance* CMaterialInstance::clone()
	{
		CMaterialInstance* pInstance = WL_NEW(CMaterialInstance, Instance)(mpMaterial);
		return pInstance;
	}

	void CMaterialInstance::bindMaterial(CMaterial* pMaterial)
	{
		if (mpMaterial != pMaterial)
		{
			WL_DECREASE(mpMaterial);
			for (auto item : mArrayShaderIns)
			{
				SafeDelete(item);
			}
			mArrayShaderIns.clear();

			mpMaterial = pMaterial;
			if (nullptr != mpMaterial)
			{
				mpMaterial->addRef();
				for (auto item : mpMaterial->mpShaders)
				{
					auto pInstance = WL_NEW(CShaderInstance, Instance)(item);
					mArrayShaderIns.emplace_back(pInstance);
					for( auto textureItem : mTextureMap )
					{
						pInstance->addTexture(textureItem.first, textureItem.second);	
					}
				}
				mRenderStateDesc.transparent = mpMaterial->mTransparent != 0 ? true : false;
				mRenderStateDesc.cullMode = mpMaterial->mCullMode;
				mRenderStateDesc.fillMode = mpMaterial->mFillMode;
				mRenderStateDesc.depthWrite = mpMaterial->mDepthWrite != 0 ? true : false;
				mRenderStateDesc.depthCompare = mpMaterial->mDepthCompare != 0 ? true : false;
				mRenderStateDesc.depth = mpMaterial->mDepth != 0 ? true : false;
				mRenderStateDesc.stencil = mpMaterial->mStencil;
				mRenderStateDesc.shadow = mpMaterial->mShadow != 0 ? true : false;
				mRenderStateDesc.enableLight = mpMaterial->mEnableLight != 0 ? true : false;
				
				setAmbientColor(pMaterial->mAmbientColor);
				setDiffuseColor(pMaterial->mDiffuseColor);
				setSpecularColor(pMaterial->mSpecularColor);
			}	
			else
			{
				mRenderStateDesc.reset();
			}
		}
	}

	CMaterial* CMaterialInstance::getMaterial() const
	{
		return mpMaterial;
	}


	void CMaterialInstance::bindTexture(TextureUseType type, CTexture* pTexture, INT32 nFlag)
	{
		if (pTexture != nullptr)
		{
			if (mTextureMap[type] != pTexture)
			{
				WL_DECREASE(mTextureMap[type]);
				mTextureMap[type] = pTexture;
				pTexture->addRef();
				for (auto shaderItem : mArrayShaderIns)
				{
					shaderItem->addTexture(type, pTexture, nFlag);
					auto pSampler = mpMaterial->getSampler(type);
					if (nullptr != pSampler )
					{
						shaderItem->setSamplers(type, pSampler);
					}
				}
			}
		}
	}


	void CMaterialInstance::bindTexture(TextureUseType type, const std::string& szTexture, INT32 nFlag)
	{
		auto pResourcePtr = GEngine->createResource(szTexture, Texture);
		if (nullptr != pResourcePtr)
		{
			auto pTexturePtr = dynamic_cast<CTexture*>(pResourcePtr);
			if (nullptr != pTexturePtr)
			{
				bindTexture(type, pTexturePtr, nFlag);
			}
		}
	}
	

	void CMaterialInstance::unBindTexture(TextureUseType type)
	{
		auto iter = mTextureMap.find(type);
		if (iter != mTextureMap.end())
		{
			WL_DECREASE(iter->second);
			mTextureMap[type] = nullptr;
			for (auto shaderItem : mArrayShaderIns)
			{
				shaderItem->removeTexture(type);
			}
		}
	}


	void CMaterialInstance::bindTexturaArray(CTextureArray* pTextureArray)
	{
		if (nullptr != pTextureArray)
		{
			bindTexturaArray(*pTextureArray);
		}
	}


	void CMaterialInstance::bindTexturaArray(const CTextureArray& pTextureArray)
	{
		mTextureArray = pTextureArray;
		for (auto shaderItem : mArrayShaderIns)
		{
			shaderItem->bindTextureArray(pTextureArray);
		}
	}

	void CMaterialInstance::unbindTextureArray()
	{
		mTextureArray.clearAllTextures();
		for (auto shaderItem : mArrayShaderIns)
		{
			shaderItem->unbindTextureArray();
		}
	}


	void CMaterialInstance::setShaderParamByName(const std::string& szName, void* pData)
	{
		//setVSParamByName(szName, pData);
		//setPSParamByName(szName, pData);
		//setHSParamByName(szName, pData);
		//setDSParamByName(szName, pData);
		//setGSParamByName(szName, pData);
		//setStructParamByName(szName, pData);
	}

	void CMaterialInstance::setVSParamByName(const std::string& szName, void* pData)
	{
		for( auto item : mArrayShaderIns )
		{
			item->setVSParamByName(szName, pData);
		}
	}

	void CMaterialInstance::setPSParamByName(const std::string& szName, void* pData)
	{
		for (auto item : mArrayShaderIns)
		{
			item->setPSParamByName(szName, pData);
		}
	}


	void CMaterialInstance::setHSParamByName(const std::string& szName, void* pData)
	{
		for (auto item : mArrayShaderIns)
		{
			item->setHSParamByName(szName, pData);
		}
	}


	void CMaterialInstance::setDSParamByName(const std::string& szName, void* pData)
	{
		for (auto item : mArrayShaderIns)
		{
			item->setDSParamByName(szName, pData);
		}
	}


	void CMaterialInstance::setGSParamByName(const std::string& szName, void* pData)
	{
		for (auto item : mArrayShaderIns)
		{
			item->setGSParamByName(szName, pData);
		}
	}


	void CMaterialInstance::setCSParamByName(const std::string& szName, void* pData)
	{
		for (auto item : mArrayShaderIns)
		{
			item->setCSParamByName(szName, pData);
		}
	}

	void CMaterialInstance::setStructParamByName(const std::string& szName, void* pData, int nSize)
	{
		for (auto item : mArrayShaderIns)
		{
			item->setStructParamByName(szName, pData, nSize);
		}
	}

	void CMaterialInstance::setTextureConstBufferParamByName(const std::string& name, void* pData)
	{
		for (auto item : mArrayShaderIns)
		{
			item->setTextureConstBufferParamByName(name, pData);
		}
	}

	void CMaterialInstance::setTextureParamByName(const std::string& name, void* pData)
	{
		for (auto item : mArrayShaderIns)
		{
			item->setTextureParamByName(name, pData);
		}
	}

	void CMaterialInstance::setTypeBufferParamByName(const std::string& name, void* pData)
	{
		for (auto item : mArrayShaderIns)
		{
			item->setTypeBufferParamByName(name, pData);
		}
	}

	void CMaterialInstance::setShaderParamByHandle(const shaderParam& nHandle, void* pData)
	{
		//setVSParamByHandle(nHandle, pData);
		//setPSParamByHandle(nHandle, pData);
		//setHSParamByHandle(nHandle, pData);
		//setDSParamByHandle(nHandle, pData);
		//setGSParamByHandle(nHandle, pData);
		//setStructParamByHandle(nHandle, pData);
	}

	void CMaterialInstance::setVSParamByHandle(const shaderParam& nHandle, void* pData)
	{
		for (auto item : mArrayShaderIns)
		{
			item->setVSParamByHandle(nHandle, pData);
		}
	}


	void CMaterialInstance::setPSParamByHandle(const shaderParam& nHandle, void* pData)
	{
		for (auto item : mArrayShaderIns)
		{
			item->setPSParamByHandle(nHandle, pData);
		}
	}


	void CMaterialInstance::setHSParamByHandle(const shaderParam& nHandle, void* pData)
	{
		for (auto item : mArrayShaderIns)
		{
			item->setHSParamByHandle(nHandle, pData);
		}
	}


	void CMaterialInstance::setDSParamByHandle(const shaderParam& nHandle, void* pData)
	{
		for (auto item : mArrayShaderIns)
		{
			item->setDSParamByHandle(nHandle, pData);
		}
	}


	void CMaterialInstance::setGSParamByHandle(const shaderParam& nHandle, void* pData)
	{
		for (auto item : mArrayShaderIns)
		{
			item->setGSParamByHandle(nHandle, pData);
		}
	}

	void CMaterialInstance::setCSParamByHandle(const shaderParam& nHandle, void* pData)
	{
		for (auto item : mArrayShaderIns)
		{
			item->setCSParamByHandle(nHandle, pData);
		}
	}

	void CMaterialInstance::setStructParamByHandle(const shaderParam& nHandle, void* pData, int nSize)
	{
		for (auto item : mArrayShaderIns)
		{
			item->setStructParamByHandle(nHandle, pData, nSize);
		}
	}

	void CMaterialInstance::setTextureConstBufferParamByHandle(const shaderParam& nHandle, void* pData)
	{
		for (auto item : mArrayShaderIns)
		{
			item->setTextureConstBufferParamByHandle(nHandle, pData);
		}
	}

	void CMaterialInstance::setTextureParamByHandle(const shaderParam& nHandle, void* pData)
	{
		for (auto item : mArrayShaderIns)
		{
			item->setTextureParamByHandle(nHandle, pData);
		}
	}

	void CMaterialInstance::setTypeBufferParamByHandle(const shaderParam& nHandle, void* pData)
	{
		for (auto item : mArrayShaderIns)
		{
			item->setTypeBufferParamByHandle(nHandle, pData);
		}
	}

	shaderParam CMaterialInstance::getVSParamByHandle(const std::string& szName, size_t nShaderIndex /*= 0*/)
	{
		shaderParam handle;
		if ( nShaderIndex < mArrayShaderIns.size())
		{
			auto iter = mArrayShaderIns.begin();
			std::advance(iter, nShaderIndex);
			handle =((*iter)->getVSParamByName(szName));
		}
		return handle;
	}


	shaderParam CMaterialInstance::getPSParamByHandle(const std::string& szName, size_t nShaderIndex /*= 0*/)
	{
		shaderParam handle;
		if (nShaderIndex < mArrayShaderIns.size())
		{
			auto iter = mArrayShaderIns.begin();
			std::advance(iter, nShaderIndex);
			handle = ((*iter)->getPSParamByName(szName));
		}
		return handle;
	}


	shaderParam CMaterialInstance::getHSParamByHandle(const std::string& szName, size_t nShaderIndex /*= 0*/)
	{
		shaderParam handle;
		if (nShaderIndex < mArrayShaderIns.size())
		{
			auto iter = mArrayShaderIns.begin();
			std::advance(iter, nShaderIndex);
			handle = ((*iter)->getHSParamByName(szName));
		}
		return handle;
	}


	shaderParam CMaterialInstance::getDSParamByHandle(const std::string& szName, size_t nShaderIndex /*= 0*/)
	{
		shaderParam handle;
		if (nShaderIndex < mArrayShaderIns.size())
		{
			auto iter = mArrayShaderIns.begin();
			std::advance(iter, nShaderIndex);
			handle = ((*iter)->getDSParamByName(szName));
		}
		return handle;
	}


	shaderParam CMaterialInstance::getGSParamByHandle(const std::string& szName, size_t nShaderIndex /*= 0*/)
	{
		shaderParam handle;
		if (nShaderIndex < mArrayShaderIns.size())
		{
			auto iter = mArrayShaderIns.begin();
			std::advance(iter, nShaderIndex);
			handle = ((*iter)->getGSParamByName(szName));
		}
		return handle;
	}

	shaderParam CMaterialInstance::getCSParamByHandle(const std::string& szName, size_t nShaderIndex /*= 0*/)
	{
		shaderParam handle;
		if (nShaderIndex < mArrayShaderIns.size())
		{
			auto iter = mArrayShaderIns.begin();
			std::advance(iter, nShaderIndex);
			handle = ((*iter)->getCSParamByName(szName));
		}
		return handle;
	}

	shaderParam CMaterialInstance::getStructParamByHandle(const std::string& szName, size_t nShaderIndex /*= 0*/)
	{
		shaderParam handle;
		if (nShaderIndex < mArrayShaderIns.size())
		{
			auto iter = mArrayShaderIns.begin();
			std::advance(iter, nShaderIndex);
			handle = ((*iter)->getStructParamByName(szName));
		}
		return handle;
	}


	std::map<TextureUseType, CTexture*>* CMaterialInstance::getTexturesPtr() 
	{
		return &mTextureMap;
	}

	void CMaterialInstance::operator= (const CMaterialInstance& instance)
	{
		if (this != &instance)
		{
			mpMaterial = instance.mpMaterial;
			mRenderStateDesc = instance.mRenderStateDesc;
			mRenderOrder = instance.mRenderOrder;
			mAmbientColor = instance.mAmbientColor;
			mDiffuseColor = instance.mDiffuseColor;
			mSpecularColor = instance.mSpecularColor;
			mpMaterial->addRef();
		}
	}

}