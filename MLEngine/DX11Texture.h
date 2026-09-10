#pragma once
#include "TextureBase.h"

namespace WL
{
	class CDX11Texture : public CTextureBase
	{
		friend class CDX11TextureArray;

	public:
		CDX11Texture();
		virtual ~CDX11Texture();
		virtual void clearRenderTargetView(DeviceContext* pContext, const Vec4F& cololr);
		void setSampleState(SamplerState* pSampleState);
		const SamplerState* getSampleState()const;
		bool initialiseShaderResourceView();
		bool initialiseTexture2D(INT32 width, INT32 height, UINT32 nFormat, UINT32 nBindFlag, UINT32 cpuAccessFlag, int usagFlag, UINT32 miscFlags);
		bool initialiseTexture1D(INT32 width, UINT32 nFormat, UINT32 nBindFlag, UINT32 cpuAccessFlag, int usagFlag, void* pData);
		bool initialiseUnorderedTexture(INT32 width, INT32 height, UINT32 nFormat, UINT32 nFlag, UINT32 nBindFlag, UINT32 cpuAccessFlag, int usagFlag, UINT mipSlice = 0, UINT32 firstArraySlice = 0, UINT32 arraySize  = -1);
		virtual bool initialise() final;
		virtual bool initialise(const SImgFileInfo& fileInfo) final;
		ID3D11RenderTargetView* getRenderTargetViewPtr() const;
		ID3D11Resource* getResourcePtr();

		ID3D11ShaderResourceView* getShaderResViewPtr();
		ID3D11UnorderedAccessView* getUnordAccessViewPtr();

		bool saveToFile(const std::string saveFile = "", DWORD nFormat = D3DX11_IFF_PNG);

		union
		{
			D3D11_TEXTURE1D_DESC mDesc1D;
			D3D11_TEXTURE2D_DESC mDesc2D;
			D3D11_TEXTURE3D_DESC mDesc3D;
		};

	protected:
		ID3D11Resource* mpTextureResource = nullptr;

		ID3D11DeviceContext* mpDeviceContext = nullptr;
		ID3D11ShaderResourceView* mpShaderResourceView = nullptr;

		ID3D11RenderTargetView* mpRenderTargetView = nullptr;
		ID3D11SamplerState* mpSampleState = nullptr;

		ID3D11UnorderedAccessView* mpTextureUAV = nullptr;
	};
}