#include "DX11Mesh.h"
#include "WLEngine.h"

namespace WL
{

	CDX11Mesh::CDX11Mesh()
	{

	}

	CDX11Mesh::~CDX11Mesh()
	{
		
	}


	void CDX11Mesh::fillVertexBuffer(void* pData, size_t length, UINT8 typeSize, int nLod /*= 0*/, int memoryType /*= USAGE_DEFAULT*/)
	{
		try
		{
			if (nullptr == pData || 0 == length)
			{
				throw 0;
			}
			mVBMemoryType = memoryType;
			//SafeDelete(mpLodVB[nLod]);
			WL_DELETE(mpLodVB[nLod], GpuBuffer)
			auto pGpuBuffer = WL_NEW(CGpuBuffer, GpuBuffer);
			pGpuBuffer->setType(mVBMemoryType);
			D3D11_BUFFER_DESC vertexBufferDesc;
			D3D11_SUBRESOURCE_DATA vertexData;
			ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));
			ZeroMemory(&vertexData, sizeof(D3D11_SUBRESOURCE_DATA));
			mVertexTypeSize[nLod] = typeSize;

			mVertexCount[nLod] = length / typeSize;
			vertexBufferDesc.Usage = (D3D11_USAGE)memoryType;
			vertexBufferDesc.ByteWidth = length;
			vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			if (D3D11_USAGE_DYNAMIC == vertexBufferDesc.Usage)
			{
				vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			}
			else
			{
				vertexBufferDesc.CPUAccessFlags = 0;
			}
			vertexBufferDesc.MiscFlags = 0;
			vertexBufferDesc.StructureByteStride = 0;

			vertexData.pSysMem = pData;
			vertexData.SysMemPitch = 0;
			vertexData.SysMemSlicePitch = 0;
			CDevice* pDevice = GEngine->getDevice();
			HRESULT result = pDevice->getGraphicsDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, &pGpuBuffer->mpBuffer);
			if (FAILED(result))
			{
				throw 1;
			}
			if (nullptr != pData)
			{
				if (typeSize == sizeof(SVertexFormatVTNTB))
				{
					mVertices.resize(mVertexCount[nLod]);
					memcpy(&mVertices[0], pData, length);
					mpDataStream = (byte*)&mVertices[0];
				}
				else if (typeSize == sizeof(SVertexVT))
				{
					mVerticesPT.resize(mVertexCount[nLod]);
					memcpy(&mVerticesPT[0], pData, length);
					mpDataStream = (byte*)&mVerticesPT[0];
				}
				else if (typeSize == sizeof(SVertexFormatVTNT))
				{
					mVerticesT.resize(mVertexCount[nLod]);
					memcpy(&mVerticesT[0], pData, length);
					mpDataStream = (byte*)&mVerticesT[0];
				}
				else if (typeSize == sizeof(SVertexFormatVTN))
				{
					mVerticesVTN.resize(mVertexCount[nLod]);
					memcpy(&mVerticesVTN[0], pData, length);
					mpDataStream = (byte*)&mVerticesVTN[0];
				}
				else
				{
					mpDataStream = WL_NEW_ARRAY(byte, length);
					memcpy(mpDataStream, pData, length);
					mbFreeDataStream = true;
				}
				mpDataLength = (INT32)length;
				
			}
			mpLodVB[nLod] = pGpuBuffer;
		}
		catch (...)
		{

		}
	}

	void CDX11Mesh::fillIndexBuffer(void* pData, size_t length, UINT8 typeSize, int nLod /*= 0*/, int memoryType /*= USAGE_IMMUTABLE*/)
	{
		try
		{
			if (nullptr == pData || 0 == length)
			{
				throw 0;
			}
			mIBMemoryType = memoryType;
			//SafeDelete(mpLodIB[nLod]);
			WL_DELETE(mpLodIB[nLod], GpuBuffer)
			mIndexCount[nLod] = length / typeSize;
			D3D11_BUFFER_DESC indexBufferDesc;
			D3D11_SUBRESOURCE_DATA indexData;
			ZeroMemory(&indexBufferDesc, sizeof(D3D11_BUFFER_DESC));
			ZeroMemory(&indexData, sizeof(D3D11_SUBRESOURCE_DATA));
			auto pGpuBuffer = WL_NEW(CGpuBuffer, GpuBuffer); 
			pGpuBuffer->setType(mIBMemoryType);
			indexBufferDesc.Usage = (D3D11_USAGE)memoryType;
			indexBufferDesc.ByteWidth = typeSize * mIndexCount[mLodLevel];
			indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			indexBufferDesc.CPUAccessFlags = 0;
			indexBufferDesc.MiscFlags = 0;
			indexBufferDesc.StructureByteStride = 0;

			indexData.pSysMem = pData;
			indexData.SysMemPitch = 0;
			indexData.SysMemSlicePitch = 0;

			CDevice* pDevice = GEngine->getDevice();
			HRESULT result = pDevice->getGraphicsDevice()->CreateBuffer(&indexBufferDesc, &indexData, &pGpuBuffer->mpBuffer);
			if (FAILED(result))
			{
				throw 1;
			}
			mIndices[nLod].resize(mIndexCount[nLod]);
			if (2 != typeSize)
			{
				memcpy(&mIndices[nLod][0], pData, length);
				mIndeiceType = DXGI_FORMAT_R32_UINT;
			}
			else
			{
				UINT16* pUINT16 = static_cast<UINT16*>(pData);
				for (UINT32 i = 0; i < mIndexCount[nLod]; ++i)
				{
					mIndices[nLod][i] = pUINT16[i];
				}
				mIndeiceType = DXGI_FORMAT_R16_UINT;
			}
			mpLodIB[nLod] = pGpuBuffer;
		}
		catch (...)
		{
		}
	}

	bool CDX11Mesh::updateVertexBuffer(DeviceContext* pDeviceContext, void* pData, size_t length, int nLod /*= 0*/)
	{
		if (USAGE_DYNAMIC == mVBMemoryType)
		{
			return mpLodVB[nLod]->updateBuffer(pDeviceContext, pData, length);
		}
		return false;
	}

	bool CDX11Mesh::updateIndexBuffer(DeviceContext* pDeviceContext, void* pData, size_t length, int nLod /*= 0*/)
	{
		if (USAGE_DYNAMIC == mIBMemoryType)
		{
			return mpLodIB[nLod]->updateBuffer(pDeviceContext, pData, length);
		}
		return false;
	}

}