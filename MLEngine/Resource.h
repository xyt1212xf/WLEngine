#pragma once
#include "Common.h"
#include "Refcount.h"
#include "GraphicHead.h"

namespace WL
{
	struct SImgFileInfo
	{
		SImgFileInfo() = default;

		SImgFileInfo(int size, int format, int width, int height, int type)
		{
			nFileSize = size;
			nFormat = format;
			nWidth = width;
			nHeight = height;
			nType = type;
		}
		union 
		{
			UINT32 nFileSize;
			UINT32 nSliceSize;
		};
		
		int nFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		int nWidth = 0;
		int nHeight = 0;
		int nDepth = 0;
		int nType = 2;
		std::string szPath = "";
	};

	enum ResourceType
	{
		Mesh,
		Texture,
		Shader,
		Material,
		Wnd,
		Font,
		Model,
		SurfaceView,
		Resource,
		UnKnow = 0XFFFF,
	};
	
	const static std::string szTextureFlag[] =
	{
		"baseTexture",
		"normalTexture",
		"detailTexture",
		"lightMapTexture",
		"textureArray",
	};


	class CResource : public CRefcount
	{
		friend class CResourceMgr;
		friend class CResourceFactory;
		friend class CUIFactory;

	public:
		CResource();
		virtual ~CResource();
		UINT32 getUUID();

		void setResourceName(const std::string& szResourceName);
		const std::string& getResourceName()const;

		void setName(const std::string& szName);
		const std::string& getName()const;
		
				
		ResourceType getType();
		std::string getTypeName() const;

		virtual bool initialise();

	protected:
		static ResourceType sResType;
		static UINT32 gUUID;
		ResourceType mType = UnKnow;
		UINT32	mUUID = 0;
		std::string mName = "";
		std::string mResourceName = "";
		std::string mTypeName = "";
	};
}