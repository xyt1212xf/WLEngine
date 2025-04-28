#pragma once
#include "Resource.h"
#include "DX11TextureArray.h"

namespace WL
{
#ifdef D3D11
	class CTextureArray : public CDX11TextureArray 
	{
		typedef CDX11TextureArray Parent;
#endif
	public:
		CTextureArray() = default;
		void use(DeviceContext* pContext);
		void setSlot(int nSlot);
		int getSlot()const;
		void operator =(const CTextureArray& textureaArray);

	private:
		int mSlot = 0;
	};
}