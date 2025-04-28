#pragma once
#include "Resource.h"
#ifdef D3D11
#include "DX11GpuBuffer.h"
#endif
namespace WL
{
#ifdef D3D11
	class CGpuBuffer : public CDX11GpuBuffer 
#endif
	{
	public:
		CGpuBuffer();
		virtual ~CGpuBuffer();
	};
}