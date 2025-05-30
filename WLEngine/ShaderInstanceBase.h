#pragma once
#include "Instance.h"
#include "Shader.h"
#include "Texture.h"
#include "TextureArray.h"

namespace WL
{
	enum GpuConstBufferType : INT32
	{
		VS_FLAG = 1 << 0,
		PS_FLAG = 1 << 1,
		HS_FLAG = 1 << 2,
		DS_FLAG = 1 << 3,
		GS_FLAG = 1 << 4,
		CS_FLAG = 1 << 5,	
		TEXT_CONST_BUF = 1 << 6,
		TEXT_BUF_FLAG = 1 << 7,
		TYPE_CONST_BUF = 1 << 8,
		STRUCT_CONST_BUF = 1 << 9,

		ALL_SHADER = VS_FLAG | PS_FLAG | HS_FLAG | DS_FLAG | GS_FLAG | CS_FLAG |
					 TEXT_CONST_BUF | TEXT_BUF_FLAG | TYPE_CONST_BUF | STRUCT_CONST_BUF
	};

	struct SShaderBuffer 
	{
		BYTE* pData = nullptr;
		void* pGpuBuffer = nullptr;
		UINT32 nSize = 0;
		UINT32 nRegister = 0xFFFFFFFF;
		std::string szName = "";
		ShaderBufferType nShaderFlags = ShaderBufferType::UNKNOW;
		bool bDirty = false;
	};

	class CShaderInstanceBase : public CInstance
	{
	public:
		CShaderInstanceBase(CShader* pShader);
		virtual ~CShaderInstanceBase();

		INT32 getTopology();
		void setTopology(INT32 nTopology);

		INT32 getCullMode();
		void setCullMode(INT32 nCullMode);

		INT32 getFillMode();
		void setFillMode(INT32 nFillMode);

		void setVSParamByName(const std::string& szName, void* pData);
		void setPSParamByName(const std::string& szName, void* pData);
		void setHSParamByName(const std::string& szName, void* pData);
		void setDSParamByName(const std::string& szName, void* pData);
		void setGSParamByName(const std::string& szName, void* pData);
		void setCSParamByName(const std::string& szName, void* pData);

		void setStructParamByName(const std::string& szName, void* pData, int nSize);
		void setTextureConstBufferParamByName(const std::string& szName, void* pData);
		void setTextureParamByName(const std::string& szName, void* pData);
		void setTypeBufferParamByName(const std::string& name, void* pData);

		void setVSParamByHandle(const SShaderParam& nHandle, void* pData);
		void setPSParamByHandle(const SShaderParam& nHandle, void* pData);
		void setHSParamByHandle(const SShaderParam& nHandle, void* pData);
		void setDSParamByHandle(const SShaderParam& nHandle, void* pData);
		void setGSParamByHandle(const SShaderParam& nHandle, void* pData);
		void setCSParamByHandle(const SShaderParam& nHandle, void* pData);
		void setStructParamByHandle(const SShaderParam& nHandle, void* pData, int nSize);
		void setTextureConstBufferParamByHandle(const SShaderParam& nHandle, void* pData);
		void setTextureParamByHandle(const SShaderParam& nHandle, void* pData);
		void setTypeBufferParamByHandle(const SShaderParam& nHandle, void* pData);

		SShaderParam getVSParamByName(const std::string& szName)const;
		SShaderParam getPSParamByName(const std::string& szName)const;
		SShaderParam getHSParamByName(const std::string& szName)const;
		SShaderParam getDSParamByName(const std::string& szName)const;
		SShaderParam getGSParamByName(const std::string& szName)const;
		SShaderParam getCSParamByName(const std::string& szName)const;
		SShaderParam getStructParamByName(const std::string& szName)const;

		void setEnableLight(bool bvalue);
		bool isEnableLight();
		void enableLight();
		void disableLight();

		void setShadow(bool bvalue);
		bool isShadow();
		void enableShadow();
		void disableShadow();

		void setZBufferWrite(bool bvalue);
		bool isZbufferWrite();
		void enableZbufferWrite();
		void disableZbufferWrite();

		void setZbufferCompare(bool bvalue);
		bool isZbufferCompare();
		void enableZbufferCompare();
		void disableZbufferCOmpare();

		void setAlpha(bool bvalue);
		bool isAlpha();
		void enableAlpha();
		void disableAlpha();
		CShader* getShader() const;
		void operator = (const CShaderInstanceBase& shaderInstance);
		void setName(const std::string& szName);
		std::string getName() const;

	protected:
		std::string mName;
		std::vector<CTexture*>	mpTextures;
		CTextureArray	mTextureArray;
		CShader* mpShader = nullptr;

		std::map<UINT32, SShaderBuffer>	mTextureConstBufferMaps;
		std::map<UINT32, SShaderBuffer>	mTextureBufferMaps;
		std::map<UINT32, SShaderBuffer>	mTypeBufferMaps;
		std::map<UINT32, SShaderBuffer>	mStructBufferMaps;

		std::map<UINT32, SShaderBuffer>	mVSDataMaps;
		std::map<UINT32, SShaderBuffer>	mHSDataMaps;
		std::map<UINT32, SShaderBuffer>	mDSDataMaps;
		std::map<UINT32, SShaderBuffer>	mPSDataMaps;
		std::map<UINT32, SShaderBuffer>	mGSDataMaps;
		std::map<UINT32, SShaderBuffer>	mCSDataMaps;

		UINT32   mTopology = PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		INT		mDepthWrite = 1;		//?????????
		INT		mDepthCompare = 1;		//?????????
		INT		mFillMode = 0;
		INT		mCullMode = 0;
		INT		mTransparent = 0;
		INT		mShadow = 0;
		INT		mEnableLight = 0;
	};
}