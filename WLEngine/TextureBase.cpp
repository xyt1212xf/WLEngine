#include "TextureBase.h"
#include "Device.h"
#include "Vector.h"
namespace WL
{

	CTextureBase::CTextureBase()
	{

	}

	CTextureBase::~CTextureBase()
	{

	}

	bool CTextureBase::initialiseTexture2D(INT32 width, INT32 height, UINT32 nFormat, UINT32 nBindFlag)
	{
		return false;
	}

	void CTextureBase::clearRenderTargetView(DeviceContext* pContext, const Vec4F& cololr)
	{

	}

	bool CTextureBase::initialise(const ImgFileInfo& fileInfo)
	{
		return false;
	}

	bool CTextureBase::saveToFile(const std::string saveFile /*= ""*/, DWORD nFormat /*= D3DX11_IFF_PNG*/)
	{
		return false;
	}

}