#include "ShaderInstanceBase.h"
namespace WL
{

	CShaderInstanceBase::CShaderInstanceBase(CShader* pShader)
	: mpShader(pShader)
	{
		if (nullptr != pShader)
		{
			mpShader->addRef();
		}
	}

	CShaderInstanceBase::~CShaderInstanceBase()
	{
		for (auto item : mTextureConstBufferMaps)
		{
			WL_DELETE_ARRAY((item.second).pData);
		}
		mTextureConstBufferMaps.clear();
		
		for (auto item : mTextureBufferMaps)
		{
			WL_DELETE_ARRAY((item.second).pData);
		}
		mTextureBufferMaps.clear();
		
		for (auto item : mTypeBufferMaps)
		{
			WL_DELETE_ARRAY((item.second).pData);
		}
		mTypeBufferMaps.clear();

		for (auto item : mStructBufferMaps)
		{
			WL_DELETE_ARRAY((item.second).pData);
		}
		mStructBufferMaps.clear();

		for (auto item : mVSDataMaps)
		{
			WL_DELETE_ARRAY((item.second).pData);
		}
		mVSDataMaps.clear();

		for (auto item : mHSDataMaps)
		{
			WL_DELETE_ARRAY((item.second).pData);
		}
		mHSDataMaps.clear();

		for (auto item : mDSDataMaps)
		{
			WL_DELETE_ARRAY((item.second).pData);
		}
		mDSDataMaps.clear();

		for (auto item : mPSDataMaps)
		{
			WL_DELETE_ARRAY((item.second).pData);
		}
		mPSDataMaps.clear();

		for (auto item : mGSDataMaps)
		{
			WL_DELETE_ARRAY((item.second).pData);
		}
		mGSDataMaps.clear();

		for (auto item : mCSDataMaps)
		{
			WL_DELETE_ARRAY((item.second).pData);
		}
		mCSDataMaps.clear();

		for (auto item : mpTextures)
		{
			if (nullptr != item)
			{
				WL_DECREASE(item);
			}
		}
		mpTextures.clear();
		WL_DECREASE(mpShader);
	}


	INT32 CShaderInstanceBase::getTopology()
	{
		return mTopology;
	}

	void CShaderInstanceBase::setTopology(INT32 nTopology)
	{
		mTopology = nTopology;
	}

	INT32 CShaderInstanceBase::getCullMode()
	{
		return mCullMode;
	}

	void CShaderInstanceBase::setCullMode(INT32 nCullMode)
	{
		mCullMode = nCullMode;
	}

	INT32 CShaderInstanceBase::getFillMode()
	{
		return mFillMode;
	}

	void CShaderInstanceBase::setFillMode(INT32 nFillMode)
	{
		mFillMode = nFillMode;
	}

	void CShaderInstanceBase::setVSParamByName(const std::string& szName, void* pData)
	{
		for (auto item : mpShader->mVSValueMap)
		{
			for (auto& shaderItem : (item.second))
			{
				if (shaderItem.szName == szName)
				{
					UINT32 nRegister = shaderItem.nRegister;
					shaderBuffer* pParm = &mVSDataMaps[nRegister];
					memcpy(pParm->pData + shaderItem.packOffset, pData, shaderItem.nSize);
					pParm->bDirty = true;
					return;
				}
			}
		}
	}

	void CShaderInstanceBase::setPSParamByName(const std::string& szName, void* pData)
	{
		for (auto item : mpShader->mPSValueMap)
		{
			for (auto& shaderItem : (item.second))
			{
				if (shaderItem.szName == szName)
				{
					UINT32 nRegister = shaderItem.nRegister;
					shaderBuffer* pParm = &mPSDataMaps[nRegister];
					memcpy(pParm->pData + shaderItem.packOffset, pData, shaderItem.nSize);
					pParm->bDirty = true;
					return;
				}
			}
		}
	}

	void CShaderInstanceBase::setHSParamByName(const std::string& szName, void* pData)
	{
		for (auto item : mpShader->mHSValueMap)
		{
			for (auto& shaderItem : (item.second))
			{
				if (shaderItem.szName == szName)
				{
					UINT32 nRegister = shaderItem.nRegister;
					shaderBuffer* pParm = &mHSDataMaps[nRegister];
					memcpy(pParm->pData + shaderItem.packOffset, pData, shaderItem.nSize);
					pParm->bDirty = true;
					return;
				}
			}
		}
	}

	void CShaderInstanceBase::setDSParamByName(const std::string& szName, void* pData)
	{
		for (auto item : mpShader->mDSValueMap)
		{
			for (auto& shaderItem : (item.second))
			{
				if (shaderItem.szName == szName)
				{
					UINT32 nRegister = shaderItem.nRegister;
					shaderBuffer* pParm = &mDSDataMaps[nRegister];
					memcpy(pParm->pData + shaderItem.packOffset, pData, shaderItem.nSize);
					pParm->bDirty = true;
					return;
				}
			}
		}
	}

	void CShaderInstanceBase::setGSParamByName(const std::string& szName, void* pData)
	{
		for (auto item : mpShader->mGSValueMap)
		{
			for (auto& shaderItem : (item.second))
			{
				if (shaderItem.szName == szName)
				{
					UINT32 nRegister = shaderItem.nRegister;
					shaderBuffer* pParm = &mGSDataMaps[nRegister];
					memcpy(pParm->pData + shaderItem.packOffset, pData, shaderItem.nSize);
					pParm->bDirty = true;
					return;
				}
			}
		}
	}

	void CShaderInstanceBase::setCSParamByName(const std::string& szName, void* pData)
	{
		for (auto item : mpShader->mCSValueMap)
		{
			for (auto& shaderItem : (item.second))
			{
				if (shaderItem.szName == szName)
				{
					UINT32 nRegister = shaderItem.nRegister;
					shaderBuffer* pParm = &mCSDataMaps[nRegister];
					memcpy(pParm->pData + shaderItem.packOffset, pData, shaderItem.nSize);
					pParm->bDirty = true;
					return;
				}
			}
		}
	}

	void CShaderInstanceBase::setStructParamByName(const std::string& szName, void* pData, int nSize)
	{
		for (auto item : mpShader->mStructValueMap)
		{
			for (auto& shaderItem : (item.second))
			{
				if (shaderItem.szName == szName)
				{
					UINT32 nRegister = shaderItem.nRegister;
					shaderBuffer* pParm = &mStructBufferMaps[nRegister];
					memcpy(pParm->pData + shaderItem.packOffset, pData, nSize);
					pParm->bDirty = true;
					return;
				}
			}
		}
	}

	void CShaderInstanceBase::setTextureConstBufferParamByName(const std::string& szName, void* pData)
	{

	}

	void CShaderInstanceBase::setTextureParamByName(const std::string& szName, void* pData)
	{
		for (auto item : mpShader->mTextureBufferValueMap)
		{
			for (auto& shaderItem : (item.second))
			{
				if (shaderItem.szName == szName)
				{
					UINT32 nRegister = shaderItem.nRegister;
					shaderBuffer* pParm = &mTextureBufferMaps[nRegister];
					memcpy(pParm->pData + shaderItem.packOffset, pData, shaderItem.nSize);
					pParm->bDirty = true;
					return;
				}
			}
		}
	}

	void CShaderInstanceBase::setTypeBufferParamByName(const std::string& name, void* pData)
	{

	}

	void CShaderInstanceBase::setVSParamByHandle(const shaderParam& handle, void* pData)
	{
		auto iter = mpShader->mVSValueMap.find(handle.nRegister);
		if (iter != mpShader->mVSValueMap.end())
		{
			for (auto& shaderItem : (iter->second))
			{
				if (shaderItem.uId == handle.uId)
				{
					UINT32 nRegister = shaderItem.nRegister;
					shaderBuffer* pParm = &mVSDataMaps[nRegister];
					pParm->bDirty = true;
					memcpy(pParm->pData + shaderItem.packOffset, pData, shaderItem.nSize);
					break;
				}
			}
		}
	}

	void CShaderInstanceBase::setPSParamByHandle(const shaderParam& handle, void* pData)
	{
		auto iter = mpShader->mPSValueMap.find(handle.nRegister);
		if (iter != mpShader->mPSValueMap.end())
		{
			for (auto& shaderItem : (iter->second))
			{
				if (shaderItem.uId == handle.uId)
				{
					UINT32 nRegister = shaderItem.nRegister;
					shaderBuffer* pParm = &mPSDataMaps[nRegister];
					pParm->bDirty = true;
					memcpy(pParm->pData + shaderItem.packOffset, pData, shaderItem.nSize);
					break;
				}
			}
		}
	}

	void CShaderInstanceBase::setHSParamByHandle(const shaderParam& handle, void* pData)
	{
		auto iter = mpShader->mHSValueMap.find(handle.nRegister);
		if (iter != mpShader->mHSValueMap.end())
		{
			for (auto& shaderItem : (iter->second))
			{
				if (shaderItem.uId == handle.uId)
				{
					UINT32 nRegister = shaderItem.nRegister;
					shaderBuffer* pParm = &mHSDataMaps[nRegister];
					pParm->bDirty = true;
					memcpy(pParm->pData + shaderItem.packOffset, pData, shaderItem.nSize);
					break;
				}
			}
		}
	}

	void CShaderInstanceBase::setDSParamByHandle(const shaderParam& handle, void* pData)
	{
		auto iter = mpShader->mDSValueMap.find(handle.nRegister);
		if (iter != mpShader->mDSValueMap.end())
		{
			for (auto& shaderItem : (iter->second))
			{
				if (shaderItem.uId == handle.uId)
				{
					UINT32 nRegister = shaderItem.nRegister;
					shaderBuffer* pParm = &mDSDataMaps[nRegister];
					pParm->bDirty = true;
					memcpy(pParm->pData + shaderItem.packOffset, pData, shaderItem.nSize);
					break;
				}
			}
		}
	}

	void CShaderInstanceBase::setGSParamByHandle(const shaderParam& handle, void* pData)
	{
		auto iter = mpShader->mGSValueMap.find(handle.nRegister);
		if (iter != mpShader->mGSValueMap.end())
		{
			for (auto& shaderItem : (iter->second))
			{
				if (shaderItem.uId == handle.uId)
				{
					UINT32 nRegister = shaderItem.nRegister;
					shaderBuffer* pParm = &mGSDataMaps[nRegister];
					pParm->bDirty = true;
					memcpy(pParm->pData + shaderItem.packOffset, pData, shaderItem.nSize);
					break;
				}
			}
		}
	}

	void CShaderInstanceBase::setCSParamByHandle(const shaderParam& nHandle, void* pData)
	{
		auto iter = mpShader->mCSValueMap.find(nHandle.nRegister);
		if (iter != mpShader->mCSValueMap.end())
		{
			for (auto& shaderItem : (iter->second))
			{
				if (shaderItem.uId == nHandle.uId)
				{
					UINT32 nRegister = shaderItem.nRegister;
					shaderBuffer* pParm = &mCSDataMaps[nRegister];
					pParm->bDirty = true;
					memcpy(pParm->pData + shaderItem.packOffset, pData, shaderItem.nSize);
					break;
				}
			}
		}

	}

	void CShaderInstanceBase::setStructParamByHandle(const shaderParam& nHandle, void* pData, int nSize)
	{
		auto iter = mpShader->mStructValueMap.find(nHandle.nRegister);
		if (iter != mpShader->mStructValueMap.end())
		{
			for (auto& shaderItem : (iter->second))
			{
				if (shaderItem.uId == nHandle.uId)
				{
					UINT32 nRegister = shaderItem.nRegister;
					shaderBuffer* pParm = &mStructBufferMaps[nRegister];
					pParm->bDirty = true;
					memcpy(pParm->pData + shaderItem.packOffset, pData, nSize);
					break;
				}
			}
		}
	}

	void CShaderInstanceBase::setTextureConstBufferParamByHandle(const shaderParam& nHandle, void* pData)
	{

	}

	void CShaderInstanceBase::setTextureParamByHandle(const shaderParam& nHandle, void* pData)
	{

	}

	void CShaderInstanceBase::setTypeBufferParamByHandle(const shaderParam& nHandle, void* pData)
	{

	}

	shaderParam CShaderInstanceBase::getVSParamByName(const std::string& szName) const
	{
		shaderParam handle;
		for (auto item : mpShader->mVSValueMap)
		{
			for (auto& child : item.second)
			{
				if (szName == (child).szName)
				{
					return child;
				}
			}
		}
		return handle;
	}

	shaderParam CShaderInstanceBase::getPSParamByName(const std::string& szName) const
	{
		shaderParam handle;
		for (auto item : mpShader->mPSValueMap)
		{
			for (auto& child : item.second)
			{
				if (szName == (child).szName)
				{
					return child;
				}
			}
		}
		return handle;
	}

	shaderParam CShaderInstanceBase::getHSParamByName(const std::string& szName) const
	{
		shaderParam handle;
		for (auto item : mpShader->mHSValueMap)
		{
			for (auto& child : item.second)
			{
				if (szName == (child).szName)
				{
					return child;
				}
			}
		}
		return handle;
	}

	shaderParam CShaderInstanceBase::getDSParamByName(const std::string& szName) const
	{
		shaderParam handle;
		for (auto item : mpShader->mDSValueMap)
		{
			for (auto& child : item.second)
			{
				if (szName == (child).szName)
				{
					return child;
				}
			}
		}
		return handle;
	}

	shaderParam CShaderInstanceBase::getGSParamByName(const std::string& szName) const
	{
		shaderParam handle;
		for (auto item : mpShader->mGSValueMap)
		{
			for (auto& child : item.second)
			{
				if (szName == (child).szName)
				{
					return child;
				}
			}
		}
		return handle;
	}

	shaderParam CShaderInstanceBase::getCSParamByName(const std::string& szName) const
	{
		shaderParam handle;
		for (auto item : mpShader->mCSValueMap)
		{
			for (auto& child : item.second)
			{
				if (szName == (child).szName)
				{
					return child;
				}
			}
		}
		return handle;

	}

	shaderParam CShaderInstanceBase::getStructParamByName(const std::string& szName) const
	{
		shaderParam handle;
		for (auto item : mpShader->mStructValueMap)
		{
			for (auto& child : item.second)
			{
				if (szName == (child).szName)
				{
					return child;
				}
			}
		}
		return handle;
	}

	void CShaderInstanceBase::setEnableLight(bool bvalue)
	{
		bvalue ? enableLight() : disableLight();
	}

	bool CShaderInstanceBase::isEnableLight()
	{
		return mEnableLight != 0 ? true : false;
	}

	void CShaderInstanceBase::enableLight()
	{
		mEnableLight = true;
	}

	void CShaderInstanceBase::disableLight()
	{
		mEnableLight = false;
	}

	void CShaderInstanceBase::setShadow(bool bvalue)
	{
		bvalue ? enableShadow() : disableShadow();
	}

	bool CShaderInstanceBase::isShadow()
	{
		return mShadow != 0 ? true : false;
	}

	void CShaderInstanceBase::enableShadow()
	{
		mShadow = 1;
	}

	void CShaderInstanceBase::disableShadow()
	{
		mShadow = 0;
	}

	void CShaderInstanceBase::setZBufferWrite(bool bvalue)
	{
		bvalue ? enableZbufferWrite() : disableZbufferWrite();
	}

	bool CShaderInstanceBase::isZbufferWrite()
	{
		return mDepthWrite != 0 ? true : false;
	}

	void CShaderInstanceBase::enableZbufferWrite()
	{
		mDepthWrite = 1;
	}

	void CShaderInstanceBase::disableZbufferWrite()
	{
		mDepthWrite = 0;
	}

	void CShaderInstanceBase::setZbufferCompare(bool bvalue)
	{
		bvalue ? enableZbufferCompare() : disableZbufferCOmpare();
	}

	bool CShaderInstanceBase::isZbufferCompare()
	{
		return mDepthCompare != 0 ? true : false;
	}

	void CShaderInstanceBase::enableZbufferCompare()
	{
		mDepthCompare = 1;
	}

	void CShaderInstanceBase::disableZbufferCOmpare()
	{
		mDepthCompare = 0;
	}

	void CShaderInstanceBase::setAlpha(bool bvalue)
	{
		bvalue ? enableAlpha() : disableAlpha();
	}

	bool CShaderInstanceBase::isAlpha()
	{
		return mTransparent != 0 ? true : false;
	}

	void CShaderInstanceBase::enableAlpha()
	{
		mTransparent = 1;
	}

	void CShaderInstanceBase::disableAlpha()
	{
		mTransparent = 0;
	}

	CShader* CShaderInstanceBase::getShader() const
	{
		return mpShader;
	}



	void CShaderInstanceBase::setName(const std::string& szName)
	{
		mName = szName;
	}

	std::string CShaderInstanceBase::getName() const
	{
		return mName;
	}

	void CShaderInstanceBase::operator=(const CShaderInstanceBase& shaderInstance)
	{
		if (this != &shaderInstance)
		{
			if (shaderInstance.mpShader != mpShader)
			{
				WL_DECREASE(mpShader)
			}
			mpShader = shaderInstance.mpShader;
			if (nullptr != mpShader)
			{
				mpShader->addRef();
			}
			mTextureConstBufferMaps = shaderInstance.mTextureConstBufferMaps;
			mTextureBufferMaps = shaderInstance.mTextureBufferMaps;
			mTypeBufferMaps = shaderInstance.mTypeBufferMaps;
			mStructBufferMaps = shaderInstance.mStructBufferMaps;

			mVSDataMaps = shaderInstance.mVSDataMaps;
			mHSDataMaps = shaderInstance.mHSDataMaps;
			mDSDataMaps = shaderInstance.mDSDataMaps;
			mPSDataMaps = shaderInstance.mPSDataMaps;
			mGSDataMaps = shaderInstance.mGSDataMaps;

			mTopology = shaderInstance.mTopology;
			mDepthWrite = shaderInstance.mDepthWrite;
			mDepthCompare = shaderInstance.mDepthCompare;
			mFillMode = shaderInstance.mFillMode;
			mCullMode = shaderInstance.mCullMode;
			mTransparent = shaderInstance.mTransparent;
			mShadow = shaderInstance.mShadow;
			mEnableLight = shaderInstance.mEnableLight;
		}
	}

}