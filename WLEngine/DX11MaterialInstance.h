#pragma once
#include "MaterialInstanceBase.h"
namespace WL
{
	class CDX11MaterialInstance : public CMaterialInstanceBase
	{
	public:
		CDX11MaterialInstance();
		virtual ~CDX11MaterialInstance();
		void setRenderState(DeviceContext* pContext);
		void commitToGpu(DeviceContext* pContext, GpuConstBufferType nFlag = ALL_SHADER);
		void commitConstBufferToGpu(DeviceContext* pContext, GpuConstBufferType nFlag = ALL_SHADER);
		void commitVSConstBufferToGpu(DeviceContext* pContext);
		void commitPSConstBufferToGpu(DeviceContext* pContext);
		void commitHSConstBufferToGpu(DeviceContext* pContext);
		void commitDSConstBufferToGpu(DeviceContext* pContext);
		void commitGSConstBufferToGpu(DeviceContext* pContext);
		void commitCSConstBufferToGpu(DeviceContext* pContext);
		void commitTextureConstBufferToGpu(DeviceContext* pContext);
		void commitTextureBufferToGpu(DeviceContext* pContext);
		void commitTypeBufferToGpu(DeviceContext* pContext);
		void commitStructBufferToGpu(DeviceContext* pContext);
	};
}