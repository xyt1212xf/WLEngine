#pragma once
#include "Instance.h"
#ifdef D3D11
#include "DX11MaterialInstance.h"
#endif
#include "TextureArray.h"

namespace WL
{
#ifdef D3D11
	class CMaterialInstance :  public CDX11MaterialInstance
#endif
	{
	public:
		CMaterialInstance(CMaterial* pMaterial);
		virtual ~CMaterialInstance();
		virtual CInstance* clone();
		void operator = (const CMaterialInstance& instance);
		void bindMaterial(CMaterial* pMaterial);
		CMaterial* getMaterial()const;
		void bindTexture(TextureUseType type, CTexture* pTexture, INT32 nFlag = ShaderBufferType::PS);
		void bindTexture(TextureUseType type, const std::string& szTexture, INT32 nFlag = ShaderBufferType::PS);
		void unBindTexture(TextureUseType type);
		void bindTexturaArray(const CTextureArray& pTextureArray);
		void bindTexturaArray(CTextureArray* pTextureArray);
		void unbindTextureArray();

		void setShaderParamByName(const std::string& szName, void* pData);
		void setVSParamByName(const std::string& szName, void* pData);
		void setPSParamByName(const std::string& szName, void* pData);
		void setHSParamByName(const std::string& szName, void* pData);
		void setDSParamByName(const std::string& szName, void* pData);
		void setGSParamByName(const std::string& szName, void* pData);
		void setCSParamByName(const std::string& szName, void* pData);
		void setStructParamByName(const std::string& szName, void* pData, int nSize);
		void setTextureConstBufferParamByName(const std::string& name, void* pData);
		void setTextureParamByName(const std::string& name, void* pData);
		void setTypeBufferParamByName(const std::string& name, void* pData);


		void setShaderParamByHandle(const shaderParam& nHandle, void* pData);
		void setVSParamByHandle(const shaderParam& nHandle, void* pData);
		void setPSParamByHandle(const shaderParam& nHandle, void* pData);
		void setHSParamByHandle(const shaderParam& nHandle, void* pData);
		void setDSParamByHandle(const shaderParam& nHandle, void* pData);
		void setGSParamByHandle(const shaderParam& nHandle, void* pData);
		void setCSParamByHandle(const shaderParam& nHandle, void* pData);
		void setStructParamByHandle(const shaderParam& nHandle, void* pData, int nSize);
		void setTextureConstBufferParamByHandle(const shaderParam& name, void* pData);
		void setTextureParamByHandle(const shaderParam& name, void* pData);
		void setTypeBufferParamByHandle(const shaderParam& name, void* pData);

		shaderParam getVSParamByHandle(const std::string& szName, size_t nShaderIndex = 0);
		shaderParam getPSParamByHandle(const std::string& szName, size_t nShaderIndex = 0);
		shaderParam getHSParamByHandle(const std::string& szName, size_t nShaderIndex = 0);
		shaderParam getDSParamByHandle(const std::string& szName, size_t nShaderIndex = 0);
		shaderParam getGSParamByHandle(const std::string& szName, size_t nShaderIndex = 0);
		shaderParam getCSParamByHandle(const std::string& szName, size_t nShaderIndex = 0);
		shaderParam getStructParamByHandle(const std::string& szName, size_t nShaderIndex = 0);
		std::map<TextureUseType, CTexture*>* getTexturesPtr();

	private:
		std::map<TextureUseType, CTexture*>	mTextureMap;
		CTextureArray mTextureArray;
	};

}