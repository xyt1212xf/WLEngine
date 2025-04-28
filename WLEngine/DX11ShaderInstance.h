#pragma once
#include "Instance.h"
#include "ShaderInstanceBase.h"
#include "Shader.h"

namespace WL
{
	class CDX11ShaderInstance : public CShaderInstanceBase
	{
	public:
		CDX11ShaderInstance(const std::string& szShaderFile);
		CDX11ShaderInstance(CShader* pShader);
		virtual ~CDX11ShaderInstance();
		//void setContext(DeviceContext* pContext);
		void setShader(DeviceContext* pContext, GpuConstBufferType nFlag = ALL_SHADER);
		bool setConstBuffer(DeviceContext* pContext, const std::string& szName, int nStart, GpuConstBufferType nFlag = ALL_SHADER);
		void setInputLayout(DeviceContext* pContext);
		void setSamplers(int nStart, SamplerState* pSampler);
		void commitToGpu(DeviceContext* pContext, GpuConstBufferType nFlag = ALL_SHADER);
		void commitDataToGpu(DeviceContext* pContext, GpuConstBufferType nFlag);

		void commitVSToGpu(DeviceContext* pContext);
		void commitPSToGpu(DeviceContext* pContext);
		void commitHSToGpu(DeviceContext* pContext);
		void commitDSToGpu(DeviceContext* pContext);
		void commitGSToGpu(DeviceContext* pContext);
		void commitCSToGpu(DeviceContext* pContext);

		void commitTextureConstBufferToGpu(DeviceContext* pContext);
		void commitTextureBufferToGpu(DeviceContext* pContext);
		void commitTypeBufferToGpu(DeviceContext* pContext);
		void commitStructBufferToGpu(DeviceContext* pContext);
	protected:
		//DeviceContext* mpContext = nullptr;
	};
}