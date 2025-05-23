#pragma once
#include "Resource.h"
#include "ShaderBase.h"
#include "GpuBuffer.h"
#include "Texture.h"

namespace WL
{
	class CDX11Shader : public CShaderBase
	{
		friend class CResourceMgr;
		friend class CDX11ShaderInstance;

	public:
		CDX11Shader();
		virtual ~CDX11Shader();
		void use(DeviceContext* pDeviceContext);
		bool createVextexShader(const std::string& fileName, const std::string& strMain, const std::string& strVersion);
		bool createVextexShader(const std::string& fileName, const std::string& strMain, const std::string& strVersion, std::vector<INPUT_ELEMENT_DESC> pInputElement);
		bool createHullShader(const std::string& fileName, const std::string& strMain, const std::string& strVersion);
		bool createDomainShader(const std::string& fileName, const std::string& strMain, const std::string& strVersion);
		bool createPixelShader(const std::string& fileName, const std::string& strMain, const std::string& strVersion);
		bool createGeometryShader(const std::string& fileName, const std::string& strMain, const std::string& strVersion);
		bool createComputeShader(const std::string& fileName, const std::string& strMain, const std::string& strVersion);
	
		bool isValidVertexShader();
		bool isValidPixelShader();
		bool isValidHullShader();
		bool isValidDomainShader();
		bool isValidGeometryShader();
		bool isValidComputeShader();
		void replaceTexture(int nRegister, CTexture* pTexture, INT32 nFlag = ShaderBufferType::PS);

	protected:
		virtual bool addPSConstBuffer(const std::string& name, INT32 size, INT32 nRegister) final; 
		virtual bool addDSConstBuffer(const std::string& name, INT32 size, INT32 nRegister) final;
		virtual bool addHSConstBuffer(const std::string& name, INT32 size, INT32 nRegister) final;
		virtual bool addVSConstBuffer(const std::string& name, INT32 size, INT32 nRegister) final;
		virtual bool addGSConstBuffer(const std::string& name, INT32 size, INT32 nRegister) final;
		virtual bool addCSConstBuffer(const std::string& name, INT32 size, INT32 nRegister, 
			int Usage, UINT32 BindFlags, UINT32 CPUAccessFlags, SSUBRESOURCE_DATA* pSubData = nullptr) final;
		virtual bool addStructedParam(const std::string& name, INT32 structSize, INT32 size, INT32 nRegister) final;
		virtual bool addTextureConstBufferParam(const std::string& name, INT32 size, INT32 nRegister) final;
		virtual bool addTextureParam(const std::string& name, INT32 nRegister, SImgFileInfo fileInfo, 
			int Usage, UINT32 BindFlags, UINT32 CPUAccessFlags ) final;
		virtual bool addTypeBufferParam(const std::string& name, INT32 size, INT32 nRegister) final;
		
		void setSampler(int nRegister, SamplerState* pSampler);
		SamplerState* getSampler(int nRegister) const;

	protected:
		ID3D11VertexShader* mpVertexShader = nullptr;
		ID3D11HullShader* mpHullShader = nullptr;
		ID3D11DomainShader* mpDomainShader = nullptr;
		ID3D11PixelShader* mpPixelShader = nullptr;
		ID3D11GeometryShader* mpGeometryShader = nullptr;
		ID3D11ComputeShader* mpComputeShader = nullptr;;
		std::vector<ID3D11InputLayout*>	mLayoutList;
		std::map<std::string, CGpuBuffer*>	mAllGpuBuffers;

		//Register,                 size,   name,        data	
		std::map<UINT32, std::tuple<UINT32, std::string, CTexture*>>	mTextureBufferMap;
		std::map<UINT32, std::tuple<UINT32, std::string, CGpuBuffer*>>	mVertexConstBufferMap;
		std::map<UINT32, std::tuple<UINT32, std::string, CGpuBuffer*>>	mTextureConstBufferMap;
		std::map<UINT32, std::tuple<UINT32, std::string, CGpuBuffer*>>	mTypeBufferMap;
		std::map<UINT32, std::tuple<UINT32, std::string, CGpuBuffer*>>	mStructBufferMap;
		std::map<UINT32, std::tuple<UINT32, std::string, CGpuBuffer*>>	mHullConstBufferMap;
		std::map<UINT32, std::tuple<UINT32, std::string, CGpuBuffer*>>	mDomainConstBufferMap;
		std::map<UINT32, std::tuple<UINT32, std::string, CGpuBuffer*>>	mPixelConstBufferMap;
		std::map<UINT32, std::tuple<UINT32, std::string, CGpuBuffer*>>	mGeometryConstBufferMap;
		std::map<UINT32, std::tuple<UINT32, std::string, CGpuBuffer*>>	mComputeConstBufferMap;

		std::map<UINT32, SamplerState*>					mTextureSample;
		std::map<UINT32, ID3D11ShaderResourceView*>		mTextureBufferResourceViewMap;
		std::map<UINT32, ID3D11ShaderResourceView*>		mTextureConstResourceViewMap;
		std::map<UINT32, ID3D11ShaderResourceView*>		mTypeResourceViewMap;
		std::map<UINT32, ID3D11ShaderResourceView*>		mStructedResourceViewMap;
		std::map<UINT32, ID3D11ShaderResourceView*>		mComputeResourceViewMap;
	};

}