#include "ShaderInstance.h"
#include "WLEngine.h"
namespace WL
{
	CShaderInstance::CShaderInstance(CShader* pShader)
	: CDX11ShaderInstance(pShader)
	{
		bindShader(pShader);
	}

	CShaderInstance::~CShaderInstance()
	{
	
	}

	void CShaderInstance::bindShader(CShader* pShader)
	{
		mTopology = pShader->mTopology;
		mDepthWrite = pShader->mDepthWrite;
		mDepthCompare = pShader->mDepthCompare;
		mFillMode = pShader->mFillMode;
		mCullMode = pShader->mCullMode;
		mTransparent = pShader->mTransparent;
		mShadow = pShader->mShadow;
		for (auto item : mpShader->mTextureBufferMap)
		{
			auto& pair = item.second;
			updateTextureBuffer(item.first, std::get<2>(pair));
		}
		for (auto item : mpShader->mVertexConstBufferMap)
		{
			auto& pair = item.second;
			updateVSConstBuffer(item.first, std::get<2>(pair));
		}
		for (auto item : mpShader->mTextureConstBufferMap)
		{
			auto& pair = item.second;
			updateTextureConstBuffer(item.first, std::get<2>(pair));
		}
		for (auto item : mpShader->mTypeBufferMap)
		{
			auto& pair = item.second;
			updateTypeBuffer(item.first, std::get<2>(pair));
		}
		for (auto item : mpShader->mStructBufferMap)
		{
			auto& pair = item.second;
			updateStructedBuffer(item.first, std::get<2>(pair));
		}
		for (auto item : mpShader->mHullConstBufferMap)
		{
			auto& pair = item.second;
			updateHSConstBuffer(item.first, std::get<2>(pair));
		}
		for (auto item : mpShader->mDomainConstBufferMap)
		{
			auto& pair = item.second;
			updateDSConstBuffer(item.first, std::get<2>(pair));
		}
		for (auto item : mpShader->mPixelConstBufferMap)
		{
			auto& pair = item.second;
			updatePSConstBuffer(item.first, std::get<2>(pair));
		}
		for (auto item : mpShader->mGeometryConstBufferMap)
		{
			auto& pair = item.second;
			updateGSConstBuffer(item.first, std::get<2>(pair));
		}
		for (auto item : mpShader->mComputeConstBufferMap)
		{
			auto& pair = item.second;
			updateCSConstBuffer(item.first, std::get<2>(pair));
		}
	}

	void CShaderInstance::bindTextureArray(const CTextureArray& textureArray)
	{
		mTextureArray = textureArray;
		
	}

	void CShaderInstance::bindTextureArray(const CTextureArray* pTextureArray)
	{
		bindTextureArray(*pTextureArray);
	}

	void CShaderInstance::unbindTextureArray()
	{
		mTextureArray.clearAllTextures();
	}

	void CShaderInstance::addTexture(int nIndex, CTexture* pTexture, INT32 nFlag/* = ShaderBufferType::PS */)
	{
		if (nullptr != pTexture)
		{
			if (nIndex >= (int)mpTextures.size())
			{
				mpTextures.resize(nIndex + 1);
			}
			mpTextures[nIndex] = pTexture;
			pTexture->addRef();
			SShaderBuffer param;
			param.nShaderFlags = (ShaderBufferType)nFlag;
			param.bDirty = false;
			param.pGpuBuffer = pTexture;
			param.nRegister = nIndex;
			param.szName = pTexture->getName();
			mTextureBufferMaps[nIndex] = param;
			mpShader->replaceTexture(nIndex, pTexture, nFlag);
		}
	}

	void CShaderInstance::bindTexture(int nIndex, CTexture* pTexture, INT32 nFlag/* = ShaderBufferType::PS*/)
	{
		if (static_cast<INT32>(mpTextures.size()) < nIndex)
		{
			if (mpTextures[nIndex] != pTexture)
			{
				WL_DECREASE(mpTextures[nIndex]);
				mpTextures[nIndex] = pTexture;
				if (nullptr != pTexture)
				{
					pTexture->addRef();
					mpShader->replaceTexture(nIndex, pTexture, nFlag);
				}
			}
		}
		else
		{
			addTexture(nIndex, pTexture);
		}
	}

	void CShaderInstance::removeTexture(int nIndex)
	{
		if (nIndex < (int)mpTextures.size())
		{
			if (mpTextures[nIndex] != nullptr)
			{
				WL_DECREASE(mpTextures[nIndex]);
				mpTextures[nIndex] = nullptr;
			}
		}
	}

	void CShaderInstance::updateVSConstBuffer(int nRegistIndex, void* pData)
	{
		if (nullptr != mpShader)
		{
			auto iter = mVSDataMaps.find(nRegistIndex);
			if (iter != mVSDataMaps.end())
			{
				memcpy((iter->second).pData, pData, (iter->second).nSize);
			}
			else
			{
				auto item = mpShader->mVertexConstBufferMap.find(nRegistIndex);
				if (item != mpShader->mVertexConstBufferMap.end())
				{
					auto& pair = item->second;
					SShaderBuffer& param = mVSDataMaps[nRegistIndex];
					param.nRegister = nRegistIndex;
					param.szName = std::get<1>(pair);
					param.nSize = std::get<0>(pair);
					
					int nSize = param.nSize;
					byte* pNewData = nullptr;
					if (nSize > 0)
					{
						pNewData = WL_NEW_ARRAY(byte, nSize);
						memcpy(pNewData, pData, nSize);
					}

					param.pData = pNewData;
					param.pGpuBuffer = std::get<2>(pair)->getBuffer();
				}
			}
		}
	}

	void CShaderInstance::updateTextureConstBuffer(int nRegistIndex, void* pData)
	{
		if (nullptr != mpShader)
		{
			auto iter = mTextureConstBufferMaps.find(nRegistIndex);
			if (iter != mTextureConstBufferMaps.end())
			{
				memcpy((iter->second).pData, pData, (iter->second).nSize);
			}
			else
			{
				auto item = mpShader->mTextureConstBufferMap.find(nRegistIndex);
				if (item != mpShader->mTextureConstBufferMap.end())
				{
					auto& pair = item->second;
					SShaderBuffer& param = mTextureConstBufferMaps[nRegistIndex];
					param.nRegister = nRegistIndex;
					param.szName = std::get<1>(pair);
					param.nSize = std::get<0>(pair);

					int nSize = param.nSize;
					byte* pNewData = nullptr;
					if (nSize > 0)
					{
						pNewData = WL_NEW_ARRAY(byte, nSize);
						memcpy(pNewData, pData, nSize);
					}

					param.pData = pNewData;
					param.pGpuBuffer = std::get<2>(pair)->getBuffer();
				}
			}
		}
	}


	void CShaderInstance::updateTextureBuffer(int nRegistIndex, void* pData)
	{
		if (nullptr != mpShader)
		{
			auto iter = mTextureBufferMaps.find(nRegistIndex);
			if (iter != mTextureBufferMaps.end())
			{
				memcpy((iter->second).pData, pData, (iter->second).nSize);
			}
			else
			{
				auto item = mpShader->mTextureBufferValueMap.find(nRegistIndex);
				auto _item = mpShader->mTextureBufferMap.find(nRegistIndex);

				if (item != mpShader->mTextureBufferValueMap.end() &&
					_item != mpShader->mTextureBufferMap.end())
				{
					SShaderBuffer& param = mTextureBufferMaps[nRegistIndex];
					auto& pair = _item->second;

					for (const auto& child : item->second)
					{
						param.nRegister = nRegistIndex;
						param.szName = child.szName;
						param.nSize = child.nSize;

						byte* pNewData = nullptr;
						int nSize = param.nSize;
						if (nSize > 0)
						{
							pNewData = WL_NEW_ARRAY(byte, nSize);
							memcpy(pNewData, pData, nSize);
						}
						param.pData = pNewData;
						param.pGpuBuffer = std::get<2>(pair);
						param.nShaderFlags = child.nShaderFlags;
					}
				}
			}
		}
	}


	void CShaderInstance::updateTypeBuffer(int nRegistIndex, void* pData)
	{
		if (nullptr != mpShader)
		{
			auto iter = mTypeBufferMaps.find(nRegistIndex);
			if (iter != mTypeBufferMaps.end())
			{
				memcpy((iter->second).pData, pData, (iter->second).nSize);
			}
			else
			{
				auto item = mpShader->mTypeBufferMap.find(nRegistIndex);
				if (item != mpShader->mTypeBufferMap.end())
				{
					auto& pair = item->second;
					SShaderBuffer& param = mTypeBufferMaps[nRegistIndex];
					param.nRegister = nRegistIndex;
					param.szName = std::get<1>(pair);
					param.nSize = std::get<0>(pair);

					int nSize = param.nSize;
					byte* pNewData = nullptr;
					if (nSize > 0)
					{
						pNewData = WL_NEW_ARRAY(byte, nSize);
						memcpy(pNewData, pData, nSize);
					}

					param.pData = pNewData;
					param.pGpuBuffer = std::get<2>(pair)->getBuffer();
				}
			}
		}
	}

	void CShaderInstance::updateHSConstBuffer(int nRegistIndex, void* pData)
	{
		if (nullptr != mpShader)
		{
			auto iter = mHSDataMaps.find(nRegistIndex);
			if (iter != mHSDataMaps.end())
			{
				memcpy((iter->second).pData, pData, (iter->second).nSize);
			}
			else
			{
				auto item = mpShader->mHullConstBufferMap.find(nRegistIndex);
				if (item != mpShader->mHullConstBufferMap.end())
				{
					auto& pair = item->second;
					SShaderBuffer& param = mHSDataMaps[nRegistIndex];
					param.nRegister = nRegistIndex;
					param.szName = std::get<1>(pair);
					param.nSize = std::get<0>(pair);

					int nSize = param.nSize;
					byte* pNewData = nullptr;
					if (nSize > 0)
					{
						pNewData = WL_NEW_ARRAY(byte, nSize);
						memcpy(pNewData, pData, nSize);
					}

					param.pData = pNewData;
					param.pGpuBuffer = std::get<2>(pair)->getBuffer();
				}
			}
		}
	}


	void CShaderInstance::updateStructedBuffer(int nRegistIndex, void* pData)
	{
		if (nullptr != mpShader)
		{
			auto iter = mStructBufferMaps.find(nRegistIndex);
			if (iter != mStructBufferMaps.end())
			{
				memcpy((iter->second).pData, pData, (iter->second).nSize);
			}
			else
			{
				auto item = mpShader->mStructBufferMap.find(nRegistIndex);
				if (item != mpShader->mStructBufferMap.end())
				{
					auto& pair = item->second;
					SShaderBuffer& param = mStructBufferMaps[nRegistIndex];
					param.nRegister = nRegistIndex;
					param.szName = std::get<1>(pair);
					param.nSize = std::get<0>(pair);

					int nSize = param.nSize;
					byte* pNewData = nullptr;
					if (nSize > 0)
					{
						pNewData = WL_NEW_ARRAY(byte, nSize);
						memcpy(pNewData, pData, nSize);
					}

					param.pData = pNewData;
					param.pGpuBuffer = std::get<2>(pair)->getBuffer();
				}
			}
		}
	}

	void CShaderInstance::updateDSConstBuffer(int nRegistIndex, void* pData)
	{
		if (nullptr != mpShader)
		{
			auto iter = mDSDataMaps.find(nRegistIndex);
			if (iter != mDSDataMaps.end())
			{
				memcpy((iter->second).pData, pData, (iter->second).nSize);
			}
			else
			{
				auto item = mpShader->mDomainConstBufferMap.find(nRegistIndex);
				if (item != mpShader->mDomainConstBufferMap.end())
				{
					auto& pair = item->second;
					SShaderBuffer& param = mDSDataMaps[nRegistIndex];
					param.nRegister = nRegistIndex;
					param.szName = std::get<1>(pair);
					param.nSize = std::get<0>(pair);

					int nSize = param.nSize;
					byte* pNewData = nullptr;
					if (nSize > 0)
					{
						pNewData = WL_NEW_ARRAY(byte, nSize);
						memcpy(pNewData, pData, nSize);
					}

					param.pData = pNewData;
					param.pGpuBuffer = std::get<2>(pair)->getBuffer();
				}
			}
		}
	}

	void CShaderInstance::updateGSConstBuffer(int nRegistIndex, void* pData)
	{
		if (nullptr != mpShader)
		{
			auto iter = mGSDataMaps.find(nRegistIndex);
			if (iter != mGSDataMaps.end())
			{
				memcpy((iter->second).pData, pData, (iter->second).nSize);
			}
			else
			{
				auto item = mpShader->mGeometryConstBufferMap.find(nRegistIndex);
				if (item != mpShader->mGeometryConstBufferMap.end())
				{
					auto& pair = item->second;
					SShaderBuffer& param = mGSDataMaps[nRegistIndex];
					param.nRegister = nRegistIndex;
					param.szName = std::get<1>(pair);
					param.nSize = std::get<0>(pair);

					int nSize = param.nSize;
					byte* pNewData = nullptr;
					if (nSize > 0)
					{
						pNewData = WL_NEW_ARRAY(byte, nSize);
						memcpy(pNewData, pData, nSize);
					}

					param.pData = pNewData;
					param.pGpuBuffer = std::get<2>(pair)->getBuffer();
				}
			}
		}
	}

	void CShaderInstance::updateCSConstBuffer(int nRegistIndex, void* pData)
	{
		if (nullptr != mpShader)
		{
			auto iter = mCSDataMaps.find(nRegistIndex);
			if (iter != mCSDataMaps.end())
			{
				memcpy((iter->second).pData, pData, (iter->second).nSize);
			}
			else
			{
				auto item = mpShader->mComputeConstBufferMap.find(nRegistIndex);
				if (item != mpShader->mComputeConstBufferMap.end())
				{
					auto& pair = item->second;
					SShaderBuffer& param = mCSDataMaps[nRegistIndex];
					param.nRegister = nRegistIndex;
					param.szName = std::get<1>(pair);
					param.nSize = std::get<0>(pair);

					int nSize = param.nSize;
					byte* pNewData = nullptr;
					if (nSize > 0)
					{
						pNewData = WL_NEW_ARRAY(byte, nSize);
						memcpy(pNewData, pData, nSize);
					}

					param.pData = pNewData;
					param.pGpuBuffer = std::get<2>(pair)->getBuffer();
				}
			}
		}
	}

	void CShaderInstance::updatePSConstBuffer(int nRegistIndex, void* pData)
	{
		if (nullptr != mpShader)
		{
			auto iter = mPSDataMaps.find(nRegistIndex);
			if (iter != mPSDataMaps.end())
			{
				memcpy((iter->second).pData, pData, (iter->second).nSize);
			}
			else
			{
				auto item = mpShader->mPixelConstBufferMap.find(nRegistIndex);
				if (item != mpShader->mPixelConstBufferMap.end())
				{
					auto& pair = item->second;
					SShaderBuffer& param = mPSDataMaps[nRegistIndex];
					param.nRegister = nRegistIndex;
					param.szName = std::get<1>(pair);
					param.nSize = std::get<0>(pair);

					int nSize = param.nSize;
					byte* pNewData = nullptr;
					if (nSize > 0)
					{
						pNewData = WL_NEW_ARRAY(byte, nSize);
						memcpy(pNewData, pData, nSize);
					}

					param.pData = pNewData;
					param.pGpuBuffer = std::get<2>(pair)->getBuffer();
				}
			}
		}
	}

	void CShaderInstance::updateVSConstBuffer(const std::string& name, void* pData)
	{
		if (nullptr != mpShader)
		{
			for (auto item : mVSDataMaps)
			{
				if (item.second.szName == name)
				{
					memcpy(item.second.pData, pData, item.second.nSize);
					return;
				}
			}
			for (auto item : mpShader->mVertexConstBufferMap)
			{
				auto& pair = item.second;
				if (std::get<1>(pair) == name)
				{
					int nRegistIndex = item.first;
					
					SShaderBuffer param;
					param.nRegister = nRegistIndex;
					param.szName = std::get<1>(pair);
					param.nSize = std::get<0>(pair);

					int nSize = param.nSize;
					byte* pNewData = nullptr;
					if (nSize > 0)
					{
						pNewData = WL_NEW_ARRAY(byte, nSize);
						memcpy(pNewData, pData, nSize);
					}

					param.pData = pNewData;
					param.pGpuBuffer = std::get<2>(pair)->getBuffer();
					mVSDataMaps[nRegistIndex] = param;
				}
			}
		}
	}

	void CShaderInstance::updateTextureConstBuffer(const std::string& name, void* pData)
	{
		if (nullptr != mpShader)
		{
			for (auto item : mTextureConstBufferMaps)
			{
				if (item.second.szName == name)
				{
					memcpy(item.second.pData, pData, item.second.nSize);
					return;
				}
			}
			for (auto item : mpShader->mTextureConstBufferMap)
			{
				auto& pair = item.second;
				if (std::get<1>(pair) == name)
				{
					int nRegistIndex = item.first;

					SShaderBuffer param;
					param.nRegister = nRegistIndex;
					param.szName = std::get<1>(pair);
					param.nSize = std::get<0>(pair);

					int nSize = param.nSize;
					byte* pNewData = nullptr;
					if (nSize > 0)
					{
						pNewData = WL_NEW_ARRAY(byte, nSize);
						memcpy(pNewData, pData, nSize);
					}

					param.pData = pNewData;
					param.pGpuBuffer = std::get<2>(pair)->getBuffer();
					mTextureConstBufferMaps[nRegistIndex] = param;
				}
			}
		}
	}

	void CShaderInstance::updateTextureBuffer(const std::string& name, void* pData)
	{
		if (nullptr != mpShader)
		{
			for (auto item : mTextureBufferMaps)
			{
				if (item.second.szName == name)
				{
					memcpy(item.second.pData, pData, item.second.nSize);
					return;
				}
			}
			for (auto item : mpShader->mTextureBufferMap)
			{
				auto& pair = item.second;
				if (std::get<1>(pair) == name)
				{
					int nRegistIndex = item.first;

					SShaderBuffer param;
					param.nRegister = nRegistIndex;
					param.szName = std::get<1>(pair);
					param.nSize = std::get<0>(pair);

					int nSize = param.nSize;
					byte* pNewData = nullptr;
					if (nSize > 0)
					{
						pNewData = WL_NEW_ARRAY(byte, nSize);
						memcpy(pNewData, pData, nSize);
					}
					param.pData = pNewData;
					param.pGpuBuffer = std::get<2>(pair);
					mTextureBufferMaps[nRegistIndex] = param;
				}
			}
		}
	}

	void CShaderInstance::updateStructedBuffer(const std::string& name, void* pData)
	{
		if (nullptr != mpShader)
		{
			for (auto item : mStructBufferMaps)
			{
				if (item.second.szName == name)
				{
					memcpy(item.second.pData, pData, item.second.nSize);
					return;
				}
			}
			for (auto item : mpShader->mStructBufferMap)
			{
				auto& pair = item.second;
				if (std::get<1>(pair) == name)
				{
					int nRegistIndex = item.first;

					SShaderBuffer param;
					param.nRegister = nRegistIndex;
					param.szName = std::get<1>(pair);
					param.nSize = std::get<0>(pair);

					int nSize = param.nSize;
					byte* pNewData = nullptr;
					if (nSize > 0)
					{
						pNewData = WL_NEW_ARRAY(byte, nSize);
						memcpy(pNewData, pData, nSize);
					}
					param.pData = pNewData;
					param.pGpuBuffer = std::get<2>(pair)->getBuffer();
					mStructBufferMaps[nRegistIndex] = param;
				}
			}
		}
	}

	void CShaderInstance::updateTypeBuffer(const std::string& name, void* pData)
	{
		if (nullptr != mpShader)
		{
			for (auto item : mTypeBufferMaps)
			{
				if (item.second.szName == name)
				{
					memcpy(item.second.pData, pData, item.second.nSize);
					return;
				}
			}
			for (auto item : mpShader->mTypeBufferMap)
			{
				auto& pair = item.second;
				if (std::get<1>(pair) == name)
				{
					int nRegistIndex = item.first;

					SShaderBuffer param;
					param.nRegister = nRegistIndex;
					param.szName = std::get<1>(pair);
					param.nSize = std::get<0>(pair);

					int nSize = param.nSize;
					byte* pNewData = nullptr;
					if (nSize > 0)
					{
						pNewData = WL_NEW_ARRAY(byte, nSize);
						memcpy(pNewData, pData, nSize);
					}
					param.pData = pNewData;
					param.pGpuBuffer = std::get<2>(pair)->getBuffer();
					mTypeBufferMaps[nRegistIndex] = param;
				}
			}
		}
	}


	void CShaderInstance::updateHSConstBuffer(const std::string& name, void* pData)
	{
		if (nullptr != mpShader)
		{
			for( auto item : mHSDataMaps )
			{
				if (item.second.szName == name)
				{
					memcpy(item.second.pData, pData, item.second.nSize);
					return;
				}
			}
			for (auto item : mpShader->mHullConstBufferMap)
			{
				auto& pair = item.second;
				if (std::get<1>(pair) == name)
				{
					int nRegistIndex = item.first;

					SShaderBuffer param;
					param.nRegister = nRegistIndex;
					param.szName = std::get<1>(pair);
					param.nSize = std::get<0>(pair);

					int nSize = param.nSize;
					byte* pNewData = nullptr;
					if (nSize > 0)
					{
						pNewData = WL_NEW_ARRAY(byte, nSize);
						memcpy(pNewData, pData, nSize);
					}

					param.pData = pNewData;
					param.pGpuBuffer = std::get<2>(pair)->getBuffer();
					mHSDataMaps[nRegistIndex] = param;
				}
			}
		}
	}




	void CShaderInstance::updateDSConstBuffer(const std::string& name, void* pData)
	{
		if (nullptr != mpShader)
		{
			for (auto item : mDSDataMaps)
			{
				if (item.second.szName == name)
				{
					memcpy(item.second.pData, pData, item.second.nSize);
					return;
				}
			}
			for (auto item : mpShader->mDomainConstBufferMap)
			{
				auto& pair = item.second;
				if (std::get<1>(pair) == name)
				{
					int nRegistIndex = item.first;

					SShaderBuffer param;
					param.nRegister = nRegistIndex;
					param.szName = std::get<1>(pair);
					param.nSize = std::get<0>(pair);

					int nSize = param.nSize;
					byte* pNewData = nullptr;
					if (nSize > 0)
					{
						pNewData = WL_NEW_ARRAY(byte, nSize);
						memcpy(pNewData, pData, nSize);
					}

					param.pData = pNewData;
					param.pGpuBuffer = std::get<2>(pair)->getBuffer();
					mDSDataMaps[nRegistIndex] = param;
				}
			}
		}
	}



	void CShaderInstance::updatePSConstBuffer(const std::string& name, void* pData)
	{
		if (nullptr != mpShader)
		{
			for (auto item : mPSDataMaps)
			{
				if (item.second.szName == name)
				{
					memcpy(item.second.pData, pData, item.second.nSize);
					return;
				}
			}
			for (auto item : mpShader->mPixelConstBufferMap)
			{
				auto& pair = item.second;
				if (std::get<1>(pair) == name)
				{
					int nRegistIndex = item.first;

					SShaderBuffer param;
					param.nRegister = nRegistIndex;
					param.szName = std::get<1>(pair);
					param.nSize = std::get<0>(pair);

					int nSize = param.nSize;
					byte* pNewData = nullptr;
					if (nSize > 0)
					{
						pNewData = WL_NEW_ARRAY(byte, nSize);
						memcpy(pNewData, pData, nSize);
					}

					param.pData = pNewData;
					param.pGpuBuffer = std::get<2>(pair)->getBuffer();
					mPSDataMaps[nRegistIndex] = param;
				}
			}
		}
	}



	void CShaderInstance::updateGSConstBuffer(const std::string& name, void* pData)
	{
		if (nullptr != mpShader)
		{
			for (auto item : mGSDataMaps)
			{
				if (item.second.szName == name)
				{
					memcpy(item.second.pData, pData, item.second.nSize);
					return;
				}
			}
			for (auto item : mpShader->mGeometryConstBufferMap)
			{
				auto& pair = item.second;
				if (std::get<1>(pair) == name)
				{
					int nRegistIndex = item.first;

					SShaderBuffer param;
					param.nRegister = nRegistIndex;
					param.szName = std::get<1>(pair);
					param.nSize = std::get<0>(pair);

					int nSize = param.nSize;
					byte* pNewData = nullptr;
					if (nSize > 0)
					{
						pNewData = WL_NEW_ARRAY(byte, nSize);
						memcpy(pNewData, pData, nSize);
					}

					param.pData = pNewData;
					param.pGpuBuffer = std::get<2>(pair);
					mGSDataMaps[nRegistIndex] = param;
				}
			}
		}
	}

	void CShaderInstance::updateCSConstBuffer(const std::string& name, void* pData)
	{
		if (nullptr != mpShader)
		{
			for (auto item : mCSDataMaps)
			{
				if (item.second.szName == name)
				{
					memcpy(item.second.pData, pData, item.second.nSize);
					return;
				}
			}
			for (auto item : mpShader->mComputeConstBufferMap)
			{
				auto& pair = item.second;
				if (std::get<1>(pair) == name)
				{
					int nRegistIndex = item.first;

					SShaderBuffer param;
					param.nRegister = nRegistIndex;
					param.szName = std::get<1>(pair);
					param.nSize = std::get<0>(pair);

					int nSize = param.nSize;
					byte* pNewData = nullptr;
					if (nSize > 0)
					{
						pNewData = WL_NEW_ARRAY(byte, nSize);
						memcpy(pNewData, pData, nSize);
					}

					param.pData = pNewData;
					param.pGpuBuffer = std::get<2>(pair);
					mCSDataMaps[nRegistIndex] = param;
				}
			}
		}
	}


}