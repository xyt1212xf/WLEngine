#include "DX11MaterialInstance.h"
#include "WLEngine.h"

namespace WL
{

	CDX11MaterialInstance::CDX11MaterialInstance()
	{

	}

	CDX11MaterialInstance::~CDX11MaterialInstance()
	{

	}

	void CDX11MaterialInstance::setRenderState(DeviceContext* pContext)
	{
		if (nullptr != mpRenderState)
		{
			GEngine->setDepthStencilState(pContext, mpRenderState, mpRenderState->nStencilRef);
			GEngine->setBlendState(pContext, mpRenderState);
			GEngine->setRasterizerState(pContext, mpRenderState);
		}
		else
		{
			GEngine->setRenderState(pContext, &mRenderStateDesc);
		}
	}

	void CDX11MaterialInstance::commitToGpu(DeviceContext* pContext, GpuConstBufferType nFlag/* = ALL_CONST_BUF*/)
	{
		setRenderState(pContext);
		for (auto& item : mArrayShaderIns)
		{
			item->commitToGpu(pContext, nFlag);
		}
	}

	void CDX11MaterialInstance::commitConstBufferToGpu(DeviceContext* pContext, GpuConstBufferType nFlag/* = ALL_CONST_BUF*/)
	{
		for (auto& item : mArrayShaderIns)
		{
			item->commitDataToGpu(pContext, nFlag);
		}
	}

	void CDX11MaterialInstance::commitVSConstBufferToGpu(DeviceContext* pContext)
	{
		for (auto& item : mArrayShaderIns)
		{
			item->commitVSToGpu(pContext);
		}
	}

	void CDX11MaterialInstance::commitPSConstBufferToGpu(DeviceContext* pContext)
	{
		for (auto& item : mArrayShaderIns)
		{
			item->commitPSToGpu(pContext);
		}
	}

	void CDX11MaterialInstance::commitHSConstBufferToGpu(DeviceContext* pContext)
	{
		for (auto& item : mArrayShaderIns)
		{
			item->commitHSToGpu(pContext);
		}
	}

	void CDX11MaterialInstance::commitDSConstBufferToGpu(DeviceContext* pContext)
	{
		for (auto& item : mArrayShaderIns)
		{
			item->commitDSToGpu(pContext);
		}
	}

	void CDX11MaterialInstance::commitGSConstBufferToGpu(DeviceContext* pContext)
	{
		for (auto& item : mArrayShaderIns)
		{
			item->commitGSToGpu(pContext);
		}
	}


	void CDX11MaterialInstance::commitCSConstBufferToGpu(DeviceContext* pContext)
	{
		for (auto& item : mArrayShaderIns)
		{
			item->commitCSToGpu(pContext);
		}
	}

	void CDX11MaterialInstance::commitTextureConstBufferToGpu(DeviceContext* pContext)
	{
		for (auto& item : mArrayShaderIns)
		{
			item->commitTextureConstBufferToGpu(pContext);
		}
	}

	void CDX11MaterialInstance::commitTextureBufferToGpu(DeviceContext* pContext)
	{
		for (auto& item : mArrayShaderIns)
		{
			item->commitTextureBufferToGpu(pContext);
		}
	}

	void CDX11MaterialInstance::commitTypeBufferToGpu(DeviceContext* pContext)
	{
		for (auto& item : mArrayShaderIns)
		{
			item->commitTypeBufferToGpu(pContext);
		}
	}

	void CDX11MaterialInstance::commitStructBufferToGpu(DeviceContext* pContext)
	{
		for (auto& item : mArrayShaderIns)
		{
			item->commitStructBufferToGpu(pContext);
		}
	}

}