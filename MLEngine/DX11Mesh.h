#pragma once
#include "meshbase.h"
#include "DX11GpuBuffer.h"

namespace WL
{
	class CDX11Mesh : public CMeshBase
	{
	public:
		CDX11Mesh();
		virtual ~CDX11Mesh();
		void fillVertexBuffer(void* pData, size_t length, UINT8 typeSize, int nLod = 0, int memoryType = USAGE_DEFAULT);
		void fillIndexBuffer(void* pData, size_t length, UINT8 typeSize, int nLod = 0, int memoryType = USAGE_IMMUTABLE);
		bool updateVertexBuffer(DeviceContext* pDeviceContext, void* pData, size_t length, int nLod = 0);
		bool updateIndexBuffer(DeviceContext* pDeviceContext, void* pData, size_t length, int nLod = 0);

	private:
		int mVBMemoryType = USAGE_DEFAULT;
		int mIBMemoryType = USAGE_IMMUTABLE;
	};
}