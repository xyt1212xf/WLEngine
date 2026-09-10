#pragma once
#include "Resource.h"
#include "Device.h"
#ifdef D3D11
#include "DX11Shader.h"
#endif

namespace WL
{
#ifdef D3D11
	class CShader : public CDX11Shader
#endif
	{
		friend class CMaterialInstance;
		friend class CShaderInstance;
		friend class CResourceMgr;
	public:
		CShader() = default;
		CShader(CDevice* pDevice);
		virtual ~CShader();
	};

}