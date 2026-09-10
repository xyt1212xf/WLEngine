#pragma once
#include "TextureBase.h"

namespace WL
{
	class CDX12Texture : public CTextureBase
	{
		friend class CDX12Texture;

	public:
		CDX12Texture();
		virtual ~CDX12Texture();
	};
}