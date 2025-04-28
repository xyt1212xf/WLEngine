#include "ShaderBase.h"
namespace WL
{
	static UINT32 gIndex = 0;
	CShaderBase::CShaderBase()
	{

	}

	CShaderBase::~CShaderBase()
	{

	}

	void CShaderBase::setDevice(CDevice* pDevice)
	{
		mpDevice = pDevice;
	}

	INT32 CShaderBase::getTopology() 
	{
		return mTopology;
	}

	void CShaderBase::setTopology(INT32 nTopology)
	{
		mTopology = nTopology;
	}

	INT32 CShaderBase::getCullMode()
	{
		return mCullMode;
	}

	void CShaderBase::setCullMode(INT32 nCullMode)
	{
		mCullMode = nCullMode;
	}

	INT32 CShaderBase::getFillMode()
	{
		return mFillMode;
	}

	void CShaderBase::setFillMode(INT32 nFillMode)
	{
		mFillMode = nFillMode;
	}

	void CShaderBase::setEnableLight(bool bvalue)
	{
		bvalue ? enableLight() : disableLight();
	}

	bool CShaderBase::isEnableLight()
	{
		return mEnableLight != 0 ? true : false;
	}

	void CShaderBase::enableLight()
	{
		mEnableLight = true;
	}

	void CShaderBase::disableLight()
	{
		mEnableLight = false;
	}

	void CShaderBase::setShadow(bool bvalue)
	{
		bvalue ? enableShadow() : disableShadow();
	}

	bool CShaderBase::isShadow()
	{
		return mShadow != 0 ? true : false;
	}

	void CShaderBase::enableShadow()
	{
		mShadow = 1;
	}

	void CShaderBase::disableShadow()
	{
		mShadow = 0;
	}

	void CShaderBase::setZBufferWrite(bool bvalue)
	{
		bvalue ? enableZbufferWrite() : disableZbufferWrite();
	}

	bool CShaderBase::isZbufferWrite()
	{
		return mDepthWrite != 0 ? true : false;
	}

	void CShaderBase::enableZbufferWrite()
	{
		mDepthWrite = 1;
	}

	void CShaderBase::disableZbufferWrite()
	{
		mDepthWrite = 0;
	}

	void CShaderBase::setZbufferCompare(bool bvalue)
	{
		bvalue ? enableZbufferCompare() : disableZbufferCOmpare();
	}

	bool CShaderBase::isZbufferCompare()
	{
		return mDepthCompare != 0 ? true : false;
	}

	void CShaderBase::enableZbufferCompare()
	{
		mDepthCompare = 1;
	}

	void CShaderBase::disableZbufferCOmpare()
	{
		mDepthCompare = 0;
	}

	void CShaderBase::setAlpha(bool bvalue)
	{
		bvalue ? enableAlpha() : disableAlpha();
	}

	bool CShaderBase::isAlpha()
	{
		return mTransparent != 0 ? true : false;
	}

	void CShaderBase::enableAlpha()
	{
		mTransparent = 1;
	}

	void CShaderBase::disableAlpha()
	{
		mTransparent = 0;
	}

	bool CShaderBase::addPSValue(const std::string& name, INT32 size, INT32 nRegister, INT32 nOffset)
	{
		shaderParam handle;
		handle.uId = ++gIndex;
		handle.szName = name;
		handle.nSize = size;
		handle.packOffset = nOffset;
		handle.nRegister = nRegister;
		auto& shaderHandles = mPSValueMap[nRegister];
		for( auto item : shaderHandles )
		{
			if (item.szName == name)
			{
				return false;
			}
		}
		shaderHandles.emplace_back(handle);
		return addPSConstBuffer(name, size, nRegister);
	}

	bool CShaderBase::addDSValue(const std::string& name, INT32 size, INT32 nRegister, INT32 nOffset)
	{
		shaderParam handle;
		handle.uId = ++gIndex;
		handle.szName = name;
		handle.nSize = size;
		handle.packOffset = nOffset;
		handle.nRegister = nRegister;
		auto& shaderHandles = mDSValueMap[nRegister];
		for (auto item : shaderHandles)
		{
			if (item.szName == name)
			{
				return false;
			}
		}
		shaderHandles.emplace_back(handle);
		return addDSConstBuffer(name, size, nRegister);
	}

	bool CShaderBase::addHSValue(const std::string& name, INT32 size, INT32 nRegister, INT32 nOffset)
	{
		shaderParam handle;
		handle.uId = ++gIndex;
		handle.szName = name;
		handle.nSize = size;
		handle.packOffset = nOffset;
		handle.nRegister = nRegister;
		auto& shaderHandles = mHSValueMap[nRegister];
		for (auto item : shaderHandles)
		{
			if (item.szName == name)
			{
				return false;
			}
		}
		shaderHandles.emplace_back(handle);
		return addHSConstBuffer(name, size, nRegister);
	}

	bool CShaderBase::addVSValue(const std::string& name, INT32 size, INT32 nRegister, INT32 nOffset)
	{
		shaderParam handle;
		handle.uId = ++gIndex;
		handle.szName = name;
		handle.nSize = size;
		handle.packOffset = nOffset;
		handle.nRegister = nRegister;
		auto& shaderHandles = mVSValueMap[nRegister];
		for (auto item : shaderHandles)
		{
			if (item.szName == name)
			{
				return false;
			}
		}
		shaderHandles.emplace_back(handle);
		return addVSConstBuffer(name, size, nRegister);
	}

	bool CShaderBase::addGSValue(const std::string& name, INT32 size, INT32 nRegister, INT32 nOffset)
	{
		shaderParam handle;
		handle.uId = ++gIndex;
		handle.szName = name;
		handle.nSize = size;
		handle.packOffset = nOffset;
		handle.nRegister = nRegister;
		auto& shaderHandles = mGSValueMap[nRegister];
		for (auto item : shaderHandles)
		{
			if (item.szName == name)
			{
				return false;
			}
		}
		shaderHandles.emplace_back(handle);
		return addGSConstBuffer(name, size, nRegister);
	}

	bool CShaderBase::addCSValue(const std::string& name, INT32 size, INT32 nRegister, INT32 nOffset, int Usage, UINT32 BindFlags, UINT32 CPUAccessFlags, SUBRESOURCE_DATA* pSubData)
	{
		shaderParam handle;
		handle.uId = ++gIndex;
		handle.szName = name;
		handle.nSize = size;
		handle.packOffset = nOffset;
		handle.nRegister = nRegister;

		auto& shaderHandles = mCSValueMap[nRegister];
		for (auto item : shaderHandles)
		{
			if (item.szName == name)
			{
				return false;
			}
		}
		shaderHandles.emplace_back(handle);
		return addCSConstBuffer(name, size, nRegister, Usage, BindFlags, CPUAccessFlags, pSubData);
	}

	bool CShaderBase::addStructValue(const std::string& name, INT32 size, INT32 nStructSize, INT32 nRegister, INT32 nOffset)
	{
		shaderParam handle;
		handle.uId = ++gIndex;
		handle.szName = name;
		handle.nSize = size;
		handle.nStructSize = nStructSize;
		handle.packOffset = nOffset;
		handle.nRegister = nRegister;
		auto& shaderHandles = mStructValueMap[nRegister];
		for (auto item : shaderHandles)
		{
			if (item.szName == name)
			{
				return false;
			}
		}
		shaderHandles.emplace_back(handle);
		return addStructedParam(name, nStructSize, size, nRegister);
	}

	bool CShaderBase::addTextureBufferValue(const std::string& name, int nRegister, const ImgFileInfo& fileInfo, INT32 nShaderFlags, 
		int Usage /*= USAGE_DYNAMIC*/,
		UINT32 BindFlags /*= BIND_SHADER_RESOURCE*/,
		UINT32 CPUAccessFlags /*= CPU_ACCESS_WRITE*/)
	{
		shaderParam handle;
		handle.uId = ++gIndex;
		handle.szName = name;
		handle.nRegister = nRegister;
		handle.nShaderFlags = static_cast<ShaderBufferType>(nShaderFlags);
		auto& shaderHandles = mTextureBufferValueMap[nRegister];
		for (auto item : shaderHandles)
		{
			if (item.szName == name)
			{
				return false;
			}
		}
		shaderHandles.emplace_back(handle);
		return addTextureParam(name, nRegister, fileInfo, Usage, BindFlags, CPUAccessFlags);
	}

	bool CShaderBase::addPSConstBuffer(const std::string& name, INT32 size, INT32 nRegister)
	{
		return false;
	}

	bool CShaderBase::addDSConstBuffer(const std::string& name, INT32 size, INT32 nRegister)
	{
		return false;
	}

	bool CShaderBase::addHSConstBuffer(const std::string& name, INT32 size, INT32 nRegister)
	{
		return false;
	}

	bool CShaderBase::addVSConstBuffer(const std::string& name, INT32 size, INT32 nRegister)
	{
		return false;
	}

	bool CShaderBase::addGSConstBuffer(const std::string& name, INT32 size, INT32 nRegister)
	{
		return false;
	}

	bool CShaderBase::addCSConstBuffer(const std::string& name, INT32 size, INT32 nRegister, int Usage, UINT32 BindFlags, UINT32 CPUAccessFlags, SUBRESOURCE_DATA* pSubData )
	{
		return false;
	}

	bool CShaderBase::addTextureConstBufferParam(const std::string& name, INT32 size, INT32 nRegister)
	{
		return false;
	}

	bool CShaderBase::addStructedParam(const std::string& name, INT32 structSize, INT32 size, INT32 nRegister)
	{
		return false;
	}

	bool CShaderBase::addTextureParam(const std::string& name, INT32 nRegister, ImgFileInfo fileInfo, 
		int Usage, UINT32 BindFlags, UINT32 CPUAccessFlags)
	{
		return false;
	}

	bool CShaderBase::addTypeBufferParam(const std::string& name, INT32 size, INT32 nRegister)
	{
		return false;
	}

}