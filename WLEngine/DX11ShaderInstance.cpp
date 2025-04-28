#include "DX11ShaderInstance.h"
#include "WLEngine.h"

namespace WL
{
	CDX11ShaderInstance::CDX11ShaderInstance(CShader* pShader)
	: CShaderInstanceBase(pShader)
	{
	}

	CDX11ShaderInstance::CDX11ShaderInstance(const std::string& szShaderFile)
	: CShaderInstanceBase(nullptr)
	{
		CShader* pShader = dynamic_cast<CShader*>(GEngine->createResource(Shader));
		mpShader = pShader;
	}

	CDX11ShaderInstance::~CDX11ShaderInstance()
	{

	}

	void CDX11ShaderInstance::setShader(DeviceContext* pContext, GpuConstBufferType nFlag /*= ALL_CONST_BUF*/)
	{
		if (nFlag & VS_FLAG)
		{
			pContext->VSSetShader(mpShader->mpVertexShader, NULL, 0);
		}
		if (nFlag & PS_FLAG)
		{
			pContext->PSSetShader(mpShader->mpPixelShader, NULL, 0);
		}
		if (nFlag & HS_FLAG)
		{
			pContext->HSSetShader(mpShader->mpHullShader, NULL, 0);
		}
		if (nFlag & DS_FLAG)
		{
			pContext->DSSetShader(mpShader->mpDomainShader, NULL, 0);
		}
		if (nFlag & GS_FLAG)
		{
			pContext->GSSetShader(mpShader->mpGeometryShader, NULL, 0);
		}
		if (nFlag & CS_FLAG)
		{
			pContext->CSSetShader(mpShader->mpComputeShader, NULL, 0);
		}
	}

	bool CDX11ShaderInstance::setConstBuffer(DeviceContext* pContext, const std::string& szName, int nStart, GpuConstBufferType nFlag /*= ALL_CONST_BUF*/)
	{
		bool bResult = false;
		auto iter = mpShader->mAllGpuBuffers.find(szName);
		if (iter != mpShader->mAllGpuBuffers.end())
		{
			ID3D11Buffer* pGpuBuffer = (iter->second)->getBuffer();
			if (nFlag & VS_FLAG)
			{
				pContext->VSSetConstantBuffers(nStart, 1, &pGpuBuffer);
			}
			if (nFlag & PS_FLAG)
			{
				pContext->PSSetConstantBuffers(nStart, 1, &pGpuBuffer);
			}
			if (nFlag & HS_FLAG)
			{
				pContext->HSSetConstantBuffers(nStart, 1, &pGpuBuffer);
			}
			if (nFlag & DS_FLAG)
			{
				pContext->DSSetConstantBuffers(nStart, 1, &pGpuBuffer);
			}
			if (nFlag & GS_FLAG)
			{
				pContext->GSSetConstantBuffers(nStart, 1, &pGpuBuffer);
			}
			if (nFlag & CS_FLAG)
			{
				pContext->CSSetConstantBuffers(nStart, 1, &pGpuBuffer);
			}
			bResult = true;
		}
		return bResult;
	}

	void CDX11ShaderInstance::setInputLayout(DeviceContext* pContext)
	{
		if (mpShader->mLayoutList.size() > 0)
		{
			pContext->IASetInputLayout(mpShader->mLayoutList.front());
		}
	}

	void CDX11ShaderInstance::setSamplers(int nStart, SamplerState* pSampler)
	{
		mpShader->setSampler(nStart, pSampler);
	}

	void CDX11ShaderInstance::commitToGpu(DeviceContext* pContext, GpuConstBufferType nFlag/* = ALL_CONST_BUF */)
	{
		mpShader->use(pContext);
		mTextureArray.use(pContext);
		pContext->IASetPrimitiveTopology((D3D_PRIMITIVE_TOPOLOGY)mTopology);
		commitDataToGpu(pContext, nFlag);
	}

	void CDX11ShaderInstance::commitDataToGpu(DeviceContext* pContext, GpuConstBufferType nFlag)
	{
		if (nFlag & VS_FLAG)
		{
			commitVSToGpu(pContext);
		}
		if (nFlag & PS_FLAG)
		{
			commitPSToGpu(pContext);
		}
		if (nFlag & HS_FLAG)
		{
			commitHSToGpu(pContext);
		}
		if (nFlag & DS_FLAG)
		{
			commitDSToGpu(pContext);
		}
		if (nFlag & GS_FLAG)
		{
			commitGSToGpu(pContext);
		}
		if (nFlag & CS_FLAG)
		{
			commitCSToGpu(pContext);
		}
		if (nFlag & TEXT_CONST_BUF)
		{
			commitTextureConstBufferToGpu(pContext);
		}
		if (nFlag & TEXT_BUF_FLAG)
		{
			commitTextureBufferToGpu(pContext);
		}
		if (nFlag & TYPE_CONST_BUF)
		{
			commitTypeBufferToGpu(pContext);
		}
		if (nFlag & STRUCT_CONST_BUF)
		{
			commitStructBufferToGpu(pContext);
		}
	}

	void CDX11ShaderInstance::commitVSToGpu(DeviceContext* pContext)
	{
		for (auto& item : mVSDataMaps)
		{
			ID3D11Buffer* pGpuBuffer = static_cast<ID3D11Buffer*>(item.second.pGpuBuffer);
			//if (item.second.bDirty)
			{
				item.second.bDirty = false;
				D3D11_MAPPED_SUBRESOURCE mappedResource;
				if (S_OK == pContext->Map(pGpuBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource))
				{
					memcpy(mappedResource.pData, item.second.pData, item.second.nSize);
					// Unlock the constant buffer.
					pContext->Unmap(pGpuBuffer, 0);
				}
			}
			pContext->VSSetConstantBuffers(item.first, 1, &pGpuBuffer);
		}
	}

	void CDX11ShaderInstance::commitPSToGpu(DeviceContext* pContext)
	{
		for (auto& item : mPSDataMaps)
		{
			ID3D11Buffer* pGpuBuffer = static_cast<ID3D11Buffer*>(item.second.pGpuBuffer);
			//if (item.second.bDirty)
			{
				item.second.bDirty = false;
				D3D11_MAPPED_SUBRESOURCE mappedResource;
				if (S_OK == pContext->Map(pGpuBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource))
				{
					memcpy(mappedResource.pData, item.second.pData, item.second.nSize);
					// Unlock the constant buffer.
					pContext->Unmap(pGpuBuffer, 0);
				}
			}
			pContext->PSSetConstantBuffers(item.first, 1, &pGpuBuffer);
		}
	}

	void CDX11ShaderInstance::commitHSToGpu(DeviceContext* pContext)
	{
		for (auto& item : mHSDataMaps)
		{
			ID3D11Buffer* pGpuBuffer = static_cast<ID3D11Buffer*>(item.second.pGpuBuffer);
			//if (item.second.bDirty)
			{
				item.second.bDirty = false;
				D3D11_MAPPED_SUBRESOURCE mappedResource;
				if (S_OK == pContext->Map(pGpuBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource) )
				{
					memcpy(mappedResource.pData, item.second.pData, item.second.nSize);
					// Unlock the constant buffer.
					pContext->Unmap(pGpuBuffer, 0);
				}
			}
			pContext->HSSetConstantBuffers(item.first, 1, &pGpuBuffer);
		}
	}

	void CDX11ShaderInstance::commitDSToGpu(DeviceContext* pContext)
	{
		for (auto& item : mDSDataMaps)
		{
			ID3D11Buffer* pGpuBuffer = static_cast<ID3D11Buffer*>(item.second.pGpuBuffer);
			//if (item.second.bDirty)
			{
				item.second.bDirty = false;
				D3D11_MAPPED_SUBRESOURCE mappedResource;
				if (S_OK == pContext->Map(pGpuBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource))
				{
					memcpy(mappedResource.pData, item.second.pData, item.second.nSize);
					// Unlock the constant buffer.
					pContext->Unmap(pGpuBuffer, 0);
				}
			}
			pContext->DSSetConstantBuffers(item.first, 1, &pGpuBuffer);
		}
	}

	void CDX11ShaderInstance::commitGSToGpu(DeviceContext* pContext)
	{
		for (auto& item : mGSDataMaps)
		{
			ID3D11Buffer* pGpuBuffer = static_cast<ID3D11Buffer*>(item.second.pGpuBuffer);
			//if (item.second.bDirty)
			{
				item.second.bDirty = false;
				D3D11_MAPPED_SUBRESOURCE mappedResource;
				if (S_OK == pContext->Map(pGpuBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource))
				{
					memcpy(mappedResource.pData, item.second.pData, item.second.nSize);
					// Unlock the constant buffer.
					pContext->Unmap(pGpuBuffer, 0);
				}
			}
			pContext->GSSetConstantBuffers(item.first, 1, &pGpuBuffer);
		}
	}

	void CDX11ShaderInstance::commitCSToGpu(DeviceContext* pContext)
	{
		for (auto& item : mCSDataMaps)
		{
			ID3D11Buffer* pGpuBuffer = static_cast<ID3D11Buffer*>(item.second.pGpuBuffer);
			//if (item.second.bDirty)
			{
				item.second.bDirty = false;
				D3D11_MAPPED_SUBRESOURCE mappedResource;
				if (S_OK == pContext->Map(pGpuBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource))
				{
					memcpy(mappedResource.pData, item.second.pData, item.second.nSize);
					// Unlock the constant buffer.
					pContext->Unmap(pGpuBuffer, 0);
				}
			}
			pContext->CSSetConstantBuffers(item.first, 1, &pGpuBuffer);
		}
		for (auto& item : mpShader->mComputeResourceViewMap)
		{
			pContext->CSSetShaderResources(item.first, 1, &item.second);
		}
	}

	void CDX11ShaderInstance::commitTextureConstBufferToGpu(DeviceContext* pContext)
	{
		for (auto& item : mTextureConstBufferMaps)
		{
			//if (item.second.bDirty)
			{
				item.second.bDirty = false;
				ID3D11Buffer* pGpuBuffer = static_cast<ID3D11Buffer*>(item.second.pGpuBuffer);
				pContext->UpdateSubresource(pGpuBuffer, 0, nullptr, item.second.pData, 0, 0);
			}
			pContext->VSSetShaderResources(item.first, 1, &mpShader->mTextureConstResourceViewMap[item.first]);
		}
	}

	void CDX11ShaderInstance::commitTextureBufferToGpu(DeviceContext* pContext)
	{
		for (auto& item : mTextureBufferMaps)
		{
			if (item.second.bDirty)
			{
				item.second.bDirty = false;
				CTexture* pTexture = static_cast<CTexture*>(item.second.pGpuBuffer);
				auto pResource = pTexture->getResourcePtr();
				D3D11_MAPPED_SUBRESOURCE mappedResource;
				if (S_OK == pContext->Map(pResource, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource))
				{
					memcpy(mappedResource.pData, item.second.pData, item.second.nSize);

					// Unlock the constant buffer.
					pContext->Unmap(pResource, 0);
				}
			}

			auto iter = mpShader->mTextureSample.find(item.first);
			if (iter != mpShader->mTextureSample.end())
			{
				if (item.second.nShaderFlags & ShaderBufferType::VS)
				{
					pContext->VSSetShaderResources(item.first, 1, &mpShader->mTextureBufferResourceViewMap[item.first]);
					pContext->VSSetSamplers(item.first, 1, &iter->second);
				}
				if (item.second.nShaderFlags & ShaderBufferType::PS)
				{
					pContext->PSSetShaderResources(item.first, 1, &mpShader->mTextureBufferResourceViewMap[item.first]);
					pContext->PSSetSamplers(item.first, 1, &iter->second);
				}
			}
		}
	}

	void CDX11ShaderInstance::commitTypeBufferToGpu(DeviceContext* pContext)
	{
		for (auto& item : mTypeBufferMaps)
		{
			//if (item.second.bDirty)
			{
				item.second.bDirty = false;
				ID3D11Buffer* pGpuBuffer = static_cast<ID3D11Buffer*>(item.second.pGpuBuffer);
				D3D11_MAPPED_SUBRESOURCE mappedResource;
				if (S_OK == pContext->Map(pGpuBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource))
				{
					memcpy(mappedResource.pData, item.second.pData, item.second.nSize);
					pContext->Unmap(pGpuBuffer, 0);
				}
			}
			pContext->VSSetShaderResources(item.first, 1, &mpShader->mTypeResourceViewMap[item.first]);
		}
	}

	void CDX11ShaderInstance::commitStructBufferToGpu(DeviceContext* pContext)
	{
		for (auto& item : mStructBufferMaps)
		{
			//if (item.second.bDirty)
			{
				item.second.bDirty = false;
				ID3D11Buffer* pGpuBuffer = static_cast<ID3D11Buffer*>(item.second.pGpuBuffer);
				D3D11_MAPPED_SUBRESOURCE mappedResource;
				if (S_OK == pContext->Map(pGpuBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource))
				{
					memcpy(mappedResource.pData, item.second.pData, item.second.nSize);
					pContext->Unmap(pGpuBuffer, 0);
				}
			}
			pContext->PSSetShaderResources(item.first, 1, &mpShader->mStructedResourceViewMap[item.first]);
		}
	}

}