#pragma once
#include "SurfaceViewBase.h"
#include "Matrix.h"
#include "DX11Device.h"
namespace WL
{
	class CDX11SurfaceView : public CSurfaceViewBase
	{
	public:
		CDX11SurfaceView() = default;
		CDX11SurfaceView(ID3D11RenderTargetView* pRTTView, ID3D11DepthStencilView* pDepthStencil);
		virtual ~CDX11SurfaceView();
		virtual bool saveToFile(const std::string szFile) final;
		virtual bool createRenderSurface(int nWidth, int nHeight, UINT32 nTextureFormat, UINT32 depthFormat, UINT bindInfo, UINT32 miscFlags) final;
		virtual bool createBuffer(void* pData,
			UINT32 nByteWidth, UINT32 nStructStride, UINT32 nUsage, UINT32 nCpuAccessFlag, UINT32 nBindFlag, UINT32 nMiscFlag) final;

		virtual bool createUAVBuffer(void* pData,
			UINT32 nByteWidth,
			UINT32 nStructStride,
			UINT32 nUsage = USAGE_DEFAULT,
			UINT32 nCpuAccessFlag = 0,
			UINT32 nBindFlag = BIND_UNORDERED_ACCESS | BIND_SHADER_RESOURCE,
			UINT32 nMiscFlag = RESOURCE_MISC_BUFFER_STRUCTURED) final;

		virtual void clearRenderSurface(DeviceContext* pContext) final;
		virtual void setRenderSurface(DeviceContext* pContext) final;
		void bindDepthStencilView(ID3D11DepthStencilView* pDepthStencilView);
		void bindRenderTargetView(ID3D11RenderTargetView* pRenderTargetView);
		ID3D11Texture2D* getRenderTexturePtr();
		ID3D11RenderTargetView* getRenderTargetViewPtr()const;
		ID3D11Texture2D* getDepthTexturePtr()const;
		ID3D11DepthStencilView* getDepthStencilViewPtr()const;
		D3D11_VIEWPORT& getViewPort();
		D3D11_VIEWPORT* getViewPortPtr();
		ID3D11UnorderedAccessView* getUnorderedAccessViewPtr()const;
		ID3D11Buffer* getBufferPtr() const;
		ID3D11ShaderResourceView* getShaderResourceView() const;

		void setVShaderResource(DeviceContext* pContext, int nStart);
		void clearVSShaderResource(DeviceContext* pContext)const;

		void setPShaderResource(DeviceContext* pContext, int nStart);
		void clearPSShaderResource(DeviceContext* pContext)const;

		void setGShaderResource(DeviceContext* pContext, int nStart);
		void clearGSShaderResource(DeviceContext* pContext)const;

		void setHShaderResource(DeviceContext* pContext, int nStart);
		void clearHSShaderResource(DeviceContext* pContext)const;

		void setDShaderResource(DeviceContext* pContext, int nStart);
		void clearDSShaderResource(DeviceContext* pContext)const;

		void setCSUAVSurface(DeviceContext* pContext, int nStart);
		void setCShaderResource(DeviceContext* pContext, int nStart);
		void setCSConstBuffer(DeviceContext* pContext, int nStart);

		void clearCSUAVSurface(DeviceContext* pContext)const;
		void clearCSShaderResource(DeviceContext* pContext)const;
		void clearCSConstBuffer(DeviceContext* pContext)const;

	private:
		ID3D11Texture2D* mpRenderTexture = nullptr;
		ID3D11RenderTargetView* mpRenderTargetView = nullptr;
		ID3D11Texture2D* mpDepthStencilBuffer = nullptr;
		ID3D11DepthStencilView* mpDepthStencilView = nullptr;
		ID3D11UnorderedAccessView* mpUnorderedAccessView = nullptr;
		ID3D11ShaderResourceView* mpShaderResourcView = nullptr;
		ID3D11Buffer* mpBuffer = nullptr;
		D3D11_VIEWPORT mViewport;
		UINT32	mUAVStart = 0;
	};
}