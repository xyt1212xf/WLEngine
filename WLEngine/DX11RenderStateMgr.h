#pragma once
#include "RenderStateMgrBase.h"
#include "RenderState.h"

namespace WL
{
	class CDX11RenderStateMgr : public CRenderStateMgrBase
	{
	public:
		CDX11RenderStateMgr(CDevice* pDevice);
		virtual ~CDX11RenderStateMgr();
		bool setAlpha(ID3D11DeviceContext* pContext, RenderStateDesc* pRS);
		bool setDepthStencil(ID3D11DeviceContext* pContext, RenderStateDesc* pRS);
		bool setDepth(ID3D11DeviceContext* pContext, RenderStateDesc* pRS);
		bool setDepthCompare(ID3D11DeviceContext* pContext, RenderStateDesc* pRS);
		bool setFillCullMode(ID3D11DeviceContext* pContext, RenderStateDesc* pRS);
		void setDepthStencilState(ID3D11DeviceContext* pContext, RenderState* pRS, int nStencilRef = 0);
		void setBlendState(ID3D11DeviceContext* pContext, RenderState* pRS, Vec4F blendFactor = {0, 0, 0, 0}, UINT SampleMas = 0xffffffff);
		void setRasterizerState(ID3D11DeviceContext* pContext, RenderState* pRS);
		virtual RenderState* createRenderState(RenderStateDesc& renderStateDesc)final;

	private:
		void initRasterizerState();
		void initDepthStencilState();

	protected:	
		ID3D11RasterizerState* mpCurrentRasterState = nullptr;
		ID3D11BlendState* mpCurrentBlendingState = nullptr;
		ID3D11DepthStencilState* mpCurrentDepthStencilState = nullptr;


		std::vector<ID3D11RasterizerState*>	mRasterStatesArray;
		std::vector<ID3D11BlendState*>	mBlendStateArray;
		std::vector<ID3D11DepthStencilState*>	mDepthStencilStatesArray;
		ID3D11RasterizerState* mpRasterStateFrontCullingSolid = nullptr;
		ID3D11RasterizerState* mpRasterStateBackCullingSolid = nullptr;
		ID3D11RasterizerState* mpRasterStateNoCullingSolid = nullptr;
		ID3D11RasterizerState* mpRasterStateFrontCullingWireFrame = nullptr;
		ID3D11RasterizerState* mpRasterStateBackCullingWireFrame = nullptr;
		ID3D11RasterizerState* mpRasterStateNoCullingWireframe = nullptr;

		ID3D11BlendState* mpEnableAlphaBlendingState = nullptr;
		ID3D11BlendState* mpDisableAlphaBlendingState = nullptr;

		ID3D11DepthStencilState* mpEnableDepthStencilState = nullptr;
		ID3D11DepthStencilState* mpDisableDepthStencilState = nullptr;
		ID3D11DepthStencilState* mpEnableDepthState = nullptr;
		ID3D11DepthStencilState* mpEnableDepthWriteState = nullptr;
		ID3D11DepthStencilState* mpDisableDepthState = nullptr;
		ID3D11DepthStencilState* mpDisableDepthWriteState = nullptr;
		std::map<INT,ID3D11DepthStencilState*> mMapDepthCompare;
	};
}