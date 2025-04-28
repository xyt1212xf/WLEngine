#include "DX11GpuBuffer.h"

namespace WL
{

	CDX11GpuBuffer::CDX11GpuBuffer()
	{

	}


	CDX11GpuBuffer::~CDX11GpuBuffer()
	{
		DEV_RELEASE(mpBuffer);
	}

	void CDX11GpuBuffer::bindGpuBuffer(ID3D11Buffer* pBuffer)
	{
		mpBuffer = pBuffer;
	}

	ID3D11Buffer* CDX11GpuBuffer::getBuffer() 
	{
		return mpBuffer;
	}

	bool CDX11GpuBuffer::updateBuffer(DeviceContext* pDeviceContext, void* pData, size_t length)
	{
		if (USAGE_DYNAMIC == mMemoryType)
		{
			D3D11_MAPPED_SUBRESOURCE mappedResource;
			auto result = pDeviceContext->Map(mpBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
			if (FAILED(result))
			{
				return false;
			}

			// Get a pointer to the data in the vertex buffer.
			VertexVTC* verticesPtr = (VertexVTC*)mappedResource.pData;

			// Copy the data into the vertex buffer.
			memcpy(verticesPtr, pData, length);

			// Unlock the vertex buffer.
			pDeviceContext->Unmap(mpBuffer, 0);
			return true;
		}
		return false;
	}

}