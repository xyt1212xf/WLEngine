#pragma once
#include "GraphicHead.h"
#include "Resource.h"
#include "Shader.h"
#include "Texture.h"
#ifdef D3D11 
#include "DX11Material.h"
#endif
namespace WL
{
#ifdef D3D11 
	class CMaterial : public CDX11Material
#endif
	{
		friend class CMaterialInstance;
		friend class CResourceMgr;
	public:
		CMaterial();
		virtual ~CMaterial();

	};
}