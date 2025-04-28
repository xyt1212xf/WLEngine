#pragma once
#include "Resource.h"
#include "Device.h"

namespace WL
{
	enum ShaderBufferType
	{
		VS = 1 << 0,
		PS = 1 << 1,
		GS = 1 << 2,
		DS = 1 << 3,
		HS = 1 << 4,
		CS = 1 << 5,
		UNKNOW = 1 << 7,
	};
	
	struct shaderParam
	{
		UINT32 uId = 0;
		UINT32 nRegister = 0xFFFFFFFF;
		int nSize = 0;
		int nStructSize = 0;
		int packOffset = 0;
		std::string szName = "";
		ShaderBufferType nShaderFlags = ShaderBufferType::UNKNOW;
	};
	class CShaderBase : public CResource
	{
		friend class CShaderInstanceBase;
	public:
		CShaderBase();
		virtual ~CShaderBase();
		void setDevice(CDevice* pDevice);

		INT32 getTopology();
		void setTopology(INT32 nTopology);

		INT32 getCullMode();
		void setCullMode(INT32 nCullMode);

		INT32 getFillMode();
		void setFillMode(INT32 nFillMode);


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

		bool addPSValue(const std::string& name, INT32 size, INT32 nRegister, INT32 nOffset);
		bool addDSValue(const std::string& name, INT32 size, INT32 nRegister, INT32 nOffset);
		bool addHSValue(const std::string& name, INT32 size, INT32 nRegister, INT32 nOffset);
		bool addVSValue(const std::string& name, INT32 size, INT32 nRegister, INT32 nOffset);
		bool addGSValue(const std::string& name, INT32 size, INT32 nRegister, INT32 nOffset);
		bool addCSValue(const std::string& name, INT32 size, INT32 nRegister, INT32 nOffset,
			int Usage, UINT32 BindFlags, UINT32 CPUAccessFlags, SUBRESOURCE_DATA* pSubData = nullptr);
		bool addStructValue(const std::string& name, INT32 size, INT32 nStructSize, INT32 nRegister, INT32 nOffset);
		bool addTextureBufferValue(const std::string& name, int nRegister, const ImgFileInfo& fileInfo, INT32 nShaderFlags,
			int Usage = USAGE_DYNAMIC,
			UINT32 BindFlags = BIND_SHADER_RESOURCE,
			UINT32 CPUAccessFlags = CPU_ACCESS_WRITE);

	private:
		virtual bool addPSConstBuffer(const std::string& name, INT32 size, INT32 nRegister);
		virtual bool addDSConstBuffer(const std::string& name, INT32 size, INT32 nRegister);
		virtual bool addHSConstBuffer(const std::string& name, INT32 size, INT32 nRegister);
		virtual bool addVSConstBuffer(const std::string& name, INT32 size, INT32 nRegister);
		virtual bool addGSConstBuffer(const std::string& name, INT32 size, INT32 nRegister);
		virtual bool addCSConstBuffer(const std::string& name, INT32 size, INT32 nRegister,
			int Usage, UINT32 BindFlags, UINT32 CPUAccessFlags, SUBRESOURCE_DATA* pSubData = nullptr);
		virtual bool addTextureConstBufferParam(const std::string& name, INT32 size, INT32 nRegister);
		virtual bool addStructedParam(const std::string& name, INT32 structSize, INT32 size, INT32 nRegister);
		virtual bool addTextureParam(const std::string& name, INT32 nRegister, ImgFileInfo fileInfo,
			int Usage, UINT32 BindFlags, UINT32 CPUAccessFlags);
		virtual bool addTypeBufferParam(const std::string& name, INT32 size, INT32 nRegister);

	protected:
		CDevice* mpDevice = nullptr;
		UINT32   mTopology = PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		INT		mDepthWrite = 1;		//?????????
		INT		mDepthCompare = 1;		//?????????
		INT		mFillMode = 0;
		INT		mCullMode = 0;
		INT		mTransparent = 0;
		INT		mShadow = 0;
		INT		mEnableLight = 0;
		std::map<UINT32, std::list<shaderParam>> mPSValueMap;
		std::map<UINT32, std::list<shaderParam>> mDSValueMap;
		std::map<UINT32, std::list<shaderParam>> mHSValueMap;
		std::map<UINT32, std::list<shaderParam>> mVSValueMap;
		std::map<UINT32, std::list<shaderParam>> mGSValueMap;
		std::map<UINT32, std::list<shaderParam>> mCSValueMap;
		std::map<UINT32, std::list<shaderParam>> mStructValueMap;
		std::map<UINT32, std::list<shaderParam>> mTextureBufferValueMap;
		//std::map<UINT32, std::list<shaderParam>> mStructValueMap;
		//std::map<UINT32, std::list<shaderParam>> mStructValueMap;

		std::string		mVertexShaderName = "";
		std::string		mHullShaderName = "";
		std::string		mDomainShaderName = "";
		std::string		mPixelShaderName = "";
		std::string		mGeometryShaderName = "";

		UINT32	mPixelConstBufferStride = 0;
		UINT32	mDomainConstBufferStride = 0;
		UINT32	mHullConstBufferStride = 0;
		UINT32	mVertexConstBufferStride = 0;
		UINT32	mGeometryConstBufferStride = 0;
	};

}