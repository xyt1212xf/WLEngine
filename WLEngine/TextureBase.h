#pragma once
#include "Resource.h"
namespace WL
{
	enum TextureType
	{
		D1,
		D2,
		D3,
	};
	enum TextureUseType
	{
		Base = 0,
		Normal = 1,
		Detail = 2,
		LightMap = 3,

		INDEX0 = 0,
		INDEX1 = 1,
		INDEX2 = 2,
		INDEX3 = 3,
		INDEX4 = 4,
		INDEX5 = 5,
		INDEX6 = 6,
		INDEX7 = 7,
		MaxValue,
	};
	class CTextureBase : public CResource
	{
	public:
		CTextureBase();
		virtual ~CTextureBase();
		virtual bool initialiseTexture2D(INT32 width, INT32 height, UINT32 nFormat, UINT32 nBindFlag);
		virtual void clearRenderTargetView(DeviceContext* pContext, const Vec4F& cololr);
		virtual bool initialise(const SImgFileInfo& fileInfo);
		bool saveToFile(const std::string saveFile = "", DWORD nFormat = D3DX11_IFF_PNG);
		
	protected:
		TextureType mType = D2;
		SImgFileInfo mFileInfo;
	};
}