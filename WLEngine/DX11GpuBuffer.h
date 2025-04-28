#pragma once

#include "DX11Device.h"
#include "GpuBufferBase.h"

namespace WL
{
	class CDX11GpuBuffer : public CGpuBufferBase
	{
	public:
		CDX11GpuBuffer();
		virtual ~CDX11GpuBuffer();
		void bindGpuBuffer(ID3D11Buffer* pBuffer);
		ID3D11Buffer* getBuffer();
		bool updateBuffer(DeviceContext* pDeviceContext, void* pData, size_t length);

	public:
		ID3D11Buffer* mpBuffer = nullptr;
	};
}