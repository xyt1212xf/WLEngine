#pragma once
#include "Resource.h"
#ifdef D3D11
#include "DX11TextureArray.h"
#elif D3D12
#include "DX12TextureArray.h"
#endif
namespace WL
{
#ifdef D3D11
	class CTextureArray : public CDX11TextureArray 
	{
		typedef CDX11TextureArray Parent;
#else
	class CTextureArray : public CDX12TextureArray
	{
		typedef CDX12TextureArray Parent;
	
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