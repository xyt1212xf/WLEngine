#include "MaterialBase.h"
namespace WL
{

	CMaterialBase::CMaterialBase()
	{

	}

	CMaterialBase::~CMaterialBase()
	{
		for (auto item : mSampleState)
		{
			DEV_RELEASE(item);
		}
		for (auto item : mpShaders)
		{
			WL_DECREASE(item);
		}
		//for (auto item : mpTextures)
		//{
		//	SafeRelease(item);
		//}
	}

	SamplerState* CMaterialBase::getSampler(int index) const
	{
		if (index < (int)mSampleState.size())
		{
			return mSampleState[index];
		}
		else
		{
			return nullptr;
		}
	}

	bool CMaterialBase::isShadow()
	{
		return mShadow != 0;
	}

	bool CMaterialBase::isLight()
	{
		return mEnableLight != 0;
	}

	void CMaterialBase::addVSValue(const std::string& name, INT32 size, INT32 nRegister, INT32 nOffset)
	{
		for( auto& item : mpShaders )
		{
			item->addVSValue(name, size, nRegister, nOffset);
		}
	}

	void CMaterialBase::addPSValue(const std::string& name, INT32 size, INT32 nRegister, INT32 nOffset)
	{
		for (auto& item : mpShaders)
		{
			item->addPSValue(name, size, nRegister, nOffset);
		}
	}

	void CMaterialBase::addDSValue(const std::string& name, INT32 size, INT32 nRegister, INT32 nOffset)
	{
		for (auto& item : mpShaders)
		{
			item->addDSValue(name, size, nRegister, nOffset);
		}
	}

	void CMaterialBase::addHSValue(const std::string& name, INT32 size, INT32 nRegister, INT32 nOffset)
	{
		for (auto& item : mpShaders)
		{
			item->addHSValue(name, size, nRegister, nOffset);
		}
	}

	void CMaterialBase::addGSValue(const std::string& name, INT32 size, INT32 nRegister, INT32 nOffset)
	{
		for (auto& item : mpShaders)
		{
			item->addHSValue(name, size, nRegister, nOffset);
		}
	}

	void CMaterialBase::addCSValue(const std::string& name, INT32 size, INT32 nRegister, INT32 nOffset, int Usage, UINT32 BindFlags, UINT32 CPUAccessFlags, SUBRESOURCE_DATA* pSubDdata /*= nullptr*/)
	{
		for (auto& item : mpShaders)
		{
			item->addCSValue(name, size, nRegister, nOffset, Usage, BindFlags, CPUAccessFlags, pSubDdata);
		}
	}

	void CMaterialBase::addStructValue(const std::string& name, INT32 size, INT32 nStructSize, INT32 nRegister, INT32 nOffset)
	{
		for (auto& item : mpShaders)
		{
			item->addStructValue(name, size, nStructSize, nRegister, nOffset);
		}
	}

	void CMaterialBase::addTextureBufferValue(const std::string& name, int nRegister, const ImgFileInfo& fileInfo, INT32 nShaderFlags, int Usage /*= USAGE_DYNAMIC*/, UINT32 BindFlags /*= BIND_SHADER_RESOURCE*/, UINT32 CPUAccessFlags /*= CPU_ACCESS_WRITE*/)
	{
		for (auto& item : mpShaders)
		{
			item->addTextureBufferValue(name, nRegister, fileInfo, nShaderFlags, Usage, BindFlags, CPUAccessFlags);
		}
	}

	bool CMaterialBase::setSampler(DeviceContext* pContext, int nStart, int nIndex)
	{
		if (nIndex < (int)mSampleState.size())
		{
			pContext->PSSetSamplers(nStart, 1, &mSampleState[nIndex]);
			return true;
		}
		else
		{
			return false;
		}
	}

	//std::vector<CTexture*>& CMaterialBase::getTextures()
	//{
	//	return mpTextures;
	//}
}