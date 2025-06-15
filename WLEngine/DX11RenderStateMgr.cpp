#include "DX11RenderStateMgr.h"
namespace WL
{
#define MAX_DEPTH_STENCIL_MODES 9
#define MAX_RASTERIZER_MODES 6
	CDX11RenderStateMgr::CDX11RenderStateMgr(CDevice* pDevice)
	: CRenderStateMgrBase(pDevice)
	{
		initRasterizerState();
		initDepthStencilState();
	}

	CDX11RenderStateMgr::~CDX11RenderStateMgr()
	{
		DEV_RELEASE(mpRasterStateFrontCullingSolid);
		DEV_RELEASE(mpRasterStateBackCullingSolid);
		DEV_RELEASE(mpRasterStateNoCullingSolid);
		DEV_RELEASE(mpRasterStateFrontCullingWireFrame);
		DEV_RELEASE(mpRasterStateBackCullingWireFrame);
		DEV_RELEASE(mpRasterStateNoCullingWireframe);

		DEV_RELEASE(mpEnableAlphaBlendingState);
		DEV_RELEASE(mpDisableAlphaBlendingState);

		DEV_RELEASE(mpEnableDepthStencilState);
		DEV_RELEASE(mpDisableDepthStencilState);
		DEV_RELEASE(mpEnableDepthState);
		DEV_RELEASE(mpEnableDepthWriteState);
		DEV_RELEASE(mpDisableDepthState);
		DEV_RELEASE(mpDisableDepthWriteState);
		for (auto item : mMapDepthCompare)
		{
			DEV_RELEASE(item.second);
		}
		for (UINT i = 0; i < MAX_DEPTH_STENCIL_MODES; i++)
		{
			DEV_RELEASE(mDepthStencilStatesArray[i]);
		}
		for (UINT i = 0; i < MAX_RASTERIZER_MODES; i++)
		{
			DEV_RELEASE(mRasterStatesArray[i]);
		}
		mMapDepthCompare.clear();
	}

	void CDX11RenderStateMgr::initRasterizerState()
	{
		D3D11_FILL_MODE fill[MAX_RASTERIZER_MODES] =
		{
			D3D11_FILL_SOLID,
			D3D11_FILL_SOLID,
			D3D11_FILL_SOLID,
			D3D11_FILL_WIREFRAME,
			D3D11_FILL_WIREFRAME,
			D3D11_FILL_WIREFRAME,
		};
		D3D11_CULL_MODE cull[MAX_RASTERIZER_MODES] =
		{
			D3D11_CULL_NONE,
			D3D11_CULL_FRONT,
			D3D11_CULL_BACK,
			D3D11_CULL_NONE,
			D3D11_CULL_FRONT,
			D3D11_CULL_BACK,
		};
		auto pDevice = mpDevice->getGraphicsDevice();
		mRasterStatesArray.resize(MAX_RASTERIZER_MODES);
		for (UINT i = 0; i < MAX_RASTERIZER_MODES; ++i)
		{
			D3D11_RASTERIZER_DESC rasterizerState;
			rasterizerState.FillMode = fill[i];
			rasterizerState.CullMode = cull[i];
			rasterizerState.FrontCounterClockwise = false;
			rasterizerState.DepthBias = false;
			rasterizerState.DepthBiasClamp = 0;
			rasterizerState.SlopeScaledDepthBias = 0;
			rasterizerState.DepthClipEnable = true;
			rasterizerState.ScissorEnable = false;
			rasterizerState.MultisampleEnable = false;
			rasterizerState.AntialiasedLineEnable = false;

			// Create the rasterizer state from the description we just filled out.
			ID3D11RasterizerState* pState = nullptr;
			HRESULT result = pDevice->CreateRasterizerState(&rasterizerState, &pState);
			if (FAILED(result))
			{
				assert(false);
				break;
			}
			mRasterStatesArray.push_back(pState);
		}
	}

	void CDX11RenderStateMgr::initDepthStencilState()
	{
		BOOL bDepthEnable[MAX_DEPTH_STENCIL_MODES] =
		{
			FALSE,
			TRUE,
			TRUE,
			FALSE,
			TRUE,
			TRUE,
			FALSE,
			TRUE,
			TRUE
		};

		BOOL bStencilEnable[MAX_DEPTH_STENCIL_MODES] =
		{
			FALSE,
			FALSE,
			FALSE,
			TRUE,
			TRUE,
			TRUE,
			TRUE,
			TRUE,
			TRUE
		};
		D3D11_COMPARISON_FUNC compFunc[MAX_DEPTH_STENCIL_MODES] =
		{
			D3D11_COMPARISON_LESS,
			D3D11_COMPARISON_LESS,
			D3D11_COMPARISON_GREATER,
			D3D11_COMPARISON_LESS,
			D3D11_COMPARISON_LESS,
			D3D11_COMPARISON_GREATER,
			D3D11_COMPARISON_LESS,
			D3D11_COMPARISON_LESS,
			D3D11_COMPARISON_GREATER,
		};

		D3D11_STENCIL_OP FailOp[MAX_DEPTH_STENCIL_MODES] =
		{
			D3D11_STENCIL_OP_KEEP,
			D3D11_STENCIL_OP_KEEP,
			D3D11_STENCIL_OP_KEEP,

			D3D11_STENCIL_OP_INCR,
			D3D11_STENCIL_OP_INCR,
			D3D11_STENCIL_OP_INCR,

			D3D11_STENCIL_OP_KEEP,
			D3D11_STENCIL_OP_KEEP,
			D3D11_STENCIL_OP_KEEP,
		};

		D3D11_STENCIL_OP PassOp[MAX_DEPTH_STENCIL_MODES] =
		{
			D3D11_STENCIL_OP_KEEP,
			D3D11_STENCIL_OP_KEEP,
			D3D11_STENCIL_OP_KEEP,

			D3D11_STENCIL_OP_KEEP,
			D3D11_STENCIL_OP_KEEP,
			D3D11_STENCIL_OP_KEEP,

			D3D11_STENCIL_OP_INCR,
			D3D11_STENCIL_OP_INCR,
			D3D11_STENCIL_OP_INCR,
		};
		auto pDevice = mpDevice->getGraphicsDevice();
		mDepthStencilStatesArray.resize(MAX_DEPTH_STENCIL_MODES);
		for (UINT i = 0; i < MAX_DEPTH_STENCIL_MODES; ++i)
		{
			D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
			depthStencilDesc.DepthEnable = bDepthEnable[i];
			depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			depthStencilDesc.DepthFunc = compFunc[i];

			// Stencil test parameters
			depthStencilDesc.StencilEnable = bStencilEnable[i];
			depthStencilDesc.StencilReadMask = 0xFF;
			depthStencilDesc.StencilWriteMask = 0xFF;

			// Stencil operations if pixel is front-facing
			depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			depthStencilDesc.FrontFace.StencilDepthFailOp = FailOp[i];
			depthStencilDesc.FrontFace.StencilPassOp = PassOp[i];
			depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

			// Stencil operations if pixel is back-facing
			depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			depthStencilDesc.BackFace.StencilDepthFailOp = FailOp[i];
			depthStencilDesc.BackFace.StencilPassOp = PassOp[i];
			depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
			// Create the depth stencil state.
			ID3D11DepthStencilState* pState = nullptr;
			HRESULT result = pDevice->CreateDepthStencilState(&depthStencilDesc, &pState);
			
			if (FAILED(result))
			{
				assert(false);
				break;
			}
			mDepthStencilStatesArray.push_back(pState);
		}
	}


	bool CDX11RenderStateMgr::setAlpha(ID3D11DeviceContext* pContext, SRenderStateDesc* pRS)
	{
		auto pDevice = mpDevice->getGraphicsDevice();
		static float blendFactor[4] = { 0, 0, 0, 0 };
		if (pRS->transparent)
		{
			if (nullptr == mpEnableAlphaBlendingState)
			{
				D3D11_BLEND_DESC blendStateDescription;
				// Clear the blend state description.
				ZeroMemory(&blendStateDescription, sizeof(D3D11_BLEND_DESC));

				// Create an alpha enabled blend state description.
				blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
				blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
				blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
				blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
				blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
				blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
				blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
				blendStateDescription.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
				// Create the blend state using the description.
				HRESULT result = pDevice->CreateBlendState(&blendStateDescription, &mpEnableAlphaBlendingState);
				if (FAILED(result))
				{
					return false;
				}
			}
			// Turn on the alpha blending.
			pContext->OMSetBlendState(mpEnableAlphaBlendingState, blendFactor, 0xffffffff);
		}
		else
		{
			if (nullptr == mpDisableAlphaBlendingState)
			{
				D3D11_BLEND_DESC blendStateDescription;
				// Clear the blend state description.
				ZeroMemory(&blendStateDescription, sizeof(D3D11_BLEND_DESC));

				// Create an alpha enabled blend state description.
				blendStateDescription.RenderTarget[0].BlendEnable = FALSE;
				blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
				blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
				blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
				blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
				blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
				blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
				blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;

				// Modify the description to create an alpha disabled blend state description.
				blendStateDescription.RenderTarget[0].BlendEnable = FALSE;
				// Create the blend state using the description.
				HRESULT result = pDevice->CreateBlendState(&blendStateDescription, &mpDisableAlphaBlendingState);
				if (FAILED(result))
				{
					return false;
				}
			}
			pContext->OMSetBlendState(mpDisableAlphaBlendingState, blendFactor, 0xffffffff);
		}

		return true;
	}

	bool CDX11RenderStateMgr::setDepthStencil(ID3D11DeviceContext* pContext, SRenderStateDesc* pRS)
	{
		auto pDevice = mpDevice->getGraphicsDevice();
		if (pRS->stencil)
		{
			if (nullptr == mpEnableDepthStencilState)
			{
				D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
				// Initialize the description of the stencil state.
				ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

				// Set up the description of the stencil state.
				depthStencilDesc.DepthEnable = true;
				depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
				depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

				depthStencilDesc.StencilEnable = true;
				depthStencilDesc.StencilReadMask = 0xFF;
				depthStencilDesc.StencilWriteMask = 0xFF;

				// Stencil operations if pixel is front-facing.
				depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
				depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
				depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
				depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

				// Stencil operations if pixel is back-facing.
				depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
				depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
				depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
				depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
				// Create the depth stencil state.
				HRESULT result = pDevice->CreateDepthStencilState(&depthStencilDesc, &mpEnableDepthStencilState);
				if (FAILED(result))
				{
					return false;
				}
			}
			pContext->OMSetDepthStencilState(mpEnableDepthStencilState, 1);
		}
		else
		{
			if (nullptr == mpDisableDepthStencilState)
			{
				D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
				// Initialize the description of the stencil state.
				ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

				// Set up the description of the stencil state.
				depthStencilDesc.DepthEnable = false;
				depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
				depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

				depthStencilDesc.StencilEnable = false;
				depthStencilDesc.StencilReadMask = 0xFF;
				depthStencilDesc.StencilWriteMask = 0xFF;

				// Stencil operations if pixel is front-facing.
				depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
				depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
				depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
				depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

				// Stencil operations if pixel is back-facing.
				depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
				depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
				depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
				depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

				// Create the depth stencil state.
				HRESULT result = pDevice->CreateDepthStencilState(&depthStencilDesc, &mpDisableDepthStencilState);
				if (FAILED(result))
				{
					return false;
				}
			}
			pContext->OMSetDepthStencilState(mpDisableDepthStencilState, 1);
		}
		return true;
	}

	bool CDX11RenderStateMgr::setDepth(ID3D11DeviceContext* pContext, SRenderStateDesc* pRS)
	{
		auto pDevice = mpDevice->getGraphicsDevice();
		if (pRS->depth)
		{
			if(pRS->depthWrite)
			{
				// Create the depth stencil state.
				if (nullptr == mpEnableDepthWriteState)
				{
					D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
					// Initialize the description of the stencil state.
					ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

					// Set up the description of the stencil state.
					depthStencilDesc.DepthEnable = true;
					depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
					depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

					depthStencilDesc.StencilEnable = false;
					depthStencilDesc.StencilReadMask = 0xFF;
					depthStencilDesc.StencilWriteMask = 0xFF;

					// Stencil operations if pixel is front-facing.
					depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
					depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
					depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
					depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

					// Stencil operations if pixel is back-facing.
					depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
					depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
					depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
					depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

					HRESULT result = pDevice->CreateDepthStencilState(&depthStencilDesc, &mpEnableDepthWriteState);
					if (FAILED(result))
					{
						return false;
					}
				}
				pContext->OMSetDepthStencilState(mpEnableDepthWriteState, 1);
			}
			else
			{
				if (nullptr == mpEnableDepthState)
				{
					D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
					// Initialize the description of the stencil state.
					ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

					// Set up the description of the stencil state.
					depthStencilDesc.DepthEnable = true;
					depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
					depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

					depthStencilDesc.StencilEnable = false;
					depthStencilDesc.StencilReadMask = 0xFF;
					depthStencilDesc.StencilWriteMask = 0xFF;

					// Stencil operations if pixel is front-facing.
					depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
					depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
					depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
					depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

					// Stencil operations if pixel is back-facing.
					depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
					depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
					depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
					depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

					HRESULT result = pDevice->CreateDepthStencilState(&depthStencilDesc, &mpEnableDepthState);
					if (FAILED(result))
					{
						return false;
					}
				}
				pContext->OMSetDepthStencilState(mpEnableDepthState, 1);
			}
		}
		else
		{
			if (pRS->depthWrite)
			{
				if (nullptr == mpDisableDepthState)
				{
					D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
					// Initialize the description of the stencil state.
					ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

					// Set up the description of the stencil state.
					depthStencilDesc.DepthEnable = false;
					depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
					depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

					depthStencilDesc.StencilEnable = false;
					depthStencilDesc.StencilReadMask = 0xFF;
					depthStencilDesc.StencilWriteMask = 0xFF;

					// Stencil operations if pixel is front-facing.
					depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
					depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
					depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
					depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

					// Stencil operations if pixel is back-facing.
					depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
					depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
					depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
					depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

					// Create the depth stencil state.
					HRESULT result = pDevice->CreateDepthStencilState(&depthStencilDesc, &mpDisableDepthState);
					if (FAILED(result))
					{
						return false;
					}
				}
				pContext->OMSetDepthStencilState(mpDisableDepthState, 1);
			}
			else
			{
				if (nullptr == mpDisableDepthWriteState)
				{
					D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
					// Initialize the description of the stencil state.
					ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

					// Set up the description of the stencil state.
					depthStencilDesc.DepthEnable = false;
					depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
					depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

					depthStencilDesc.StencilEnable = false;
					depthStencilDesc.StencilReadMask = 0xFF;
					depthStencilDesc.StencilWriteMask = 0xFF;

					// Stencil operations if pixel is front-facing.
					depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
					depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
					depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
					depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

					// Stencil operations if pixel is back-facing.
					depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
					depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
					depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
					depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

					// Create the depth stencil state.
					HRESULT result = pDevice->CreateDepthStencilState(&depthStencilDesc, &mpDisableDepthWriteState);
					if (FAILED(result))
					{
						return false;
					}
				}
				pContext->OMSetDepthStencilState(mpDisableDepthWriteState, 1);
			}
		}
		return true;
	}

	bool CDX11RenderStateMgr::setDepthCompare(ID3D11DeviceContext* pContext, SRenderStateDesc* pRS)
	{
		return true;
	}

	bool CDX11RenderStateMgr::setFillCullMode(ID3D11DeviceContext* pContext, SRenderStateDesc* pRS)
	{
		auto pDevice = mpDevice->getGraphicsDevice();
		if (FILL_SOLID == pRS->fillMode)
		{
			if (CULL_NONE == pRS->cullMode)
			{
				if ( nullptr == mpRasterStateNoCullingSolid)
				{
					D3D11_RASTERIZER_DESC RasterizerDesc =
					{
						D3D11_FILL_SOLID,   // D3D11_FILL_MODE FillMode;
						D3D11_CULL_NONE,    // D3D11_CULL_MODE CullMode;
						TRUE,               // BOOL FrontCounterClockwise;
						0,                  // INT DepthBias;
						0,                  // FLOAT DepthBiasClamp;
						0,                  // FLOAT SlopeScaledDepthBias;
						FALSE,              // BOOL DepthClipEnable;
						FALSE,              // BOOL ScissorEnable;
						TRUE,               // BOOL MultisampleEnable;
						FALSE,              // BOOL AntialiasedLineEnable;
					};
					// Create the rasterizer state from the description we just filled out.
					HRESULT result = pDevice->CreateRasterizerState(&RasterizerDesc, &mpRasterStateNoCullingSolid);
					if (FAILED(result))
					{
						return false;
					}
				}
				pContext->RSSetState(mpRasterStateNoCullingSolid);
			}
			else if(CULL_FRONT == pRS->cullMode)
			{
				if (nullptr == mpRasterStateFrontCullingSolid)
				{
					D3D11_RASTERIZER_DESC RasterizerDesc =
					{
						D3D11_FILL_SOLID,   // D3D11_FILL_MODE FillMode;
						D3D11_CULL_FRONT,    // D3D11_CULL_MODE CullMode;
						TRUE,               // BOOL FrontCounterClockwise;
						0,                  // INT DepthBias;
						0,                  // FLOAT DepthBiasClamp;
						0,                  // FLOAT SlopeScaledDepthBias;
						FALSE,              // BOOL DepthClipEnable;
						FALSE,              // BOOL ScissorEnable;
						TRUE,               // BOOL MultisampleEnable;
						FALSE,              // BOOL AntialiasedLineEnable;
					};
					// Create the rasterizer state from the description we just filled out.
					HRESULT result = pDevice->CreateRasterizerState(&RasterizerDesc, &mpRasterStateFrontCullingSolid);
					if (FAILED(result))
					{
						return false;
					}
				}
				pContext->RSSetState(mpRasterStateFrontCullingSolid);
			}
			else
			{
				if (nullptr == mpRasterStateBackCullingSolid)
				{
					D3D11_RASTERIZER_DESC RasterizerDesc =
					{
						D3D11_FILL_SOLID,   // D3D11_FILL_MODE FillMode;
						D3D11_CULL_BACK,    // D3D11_CULL_MODE CullMode;
						TRUE,               // BOOL FrontCounterClockwise;
						0,                  // INT DepthBias;
						0,                  // FLOAT DepthBiasClamp;
						0,                  // FLOAT SlopeScaledDepthBias;
						FALSE,              // BOOL DepthClipEnable;
						FALSE,              // BOOL ScissorEnable;
						TRUE,               // BOOL MultisampleEnable;
						FALSE,              // BOOL AntialiasedLineEnable;
					};
					// Create the rasterizer state from the description we just filled out.
					HRESULT result = pDevice->CreateRasterizerState(&RasterizerDesc, &mpRasterStateBackCullingSolid);
					if (FAILED(result))
					{
						return false;
					}
				}
				pContext->RSSetState(mpRasterStateBackCullingSolid);
			}
		}
		else
		{
			if (CULL_NONE == pRS->cullMode)
			{
				if (nullptr == mpRasterStateNoCullingWireframe)
				{
					D3D11_RASTERIZER_DESC RasterizerDesc =
					{
						D3D11_FILL_WIREFRAME,   // D3D11_FILL_MODE FillMode;
						D3D11_CULL_NONE,    // D3D11_CULL_MODE CullMode;
						TRUE,               // BOOL FrontCounterClockwise;
						0,                  // INT DepthBias;
						0,                  // FLOAT DepthBiasClamp;
						0,                  // FLOAT SlopeScaledDepthBias;
						FALSE,              // BOOL DepthClipEnable;
						FALSE,              // BOOL ScissorEnable;
						TRUE,               // BOOL MultisampleEnable;
						FALSE,              // BOOL AntialiasedLineEnable;
					};
					// Create the rasterizer state from the description we just filled out.
					HRESULT result = pDevice->CreateRasterizerState(&RasterizerDesc, &mpRasterStateNoCullingWireframe);
					if (FAILED(result))
					{
						return false;
					}
				}
				pContext->RSSetState(mpRasterStateNoCullingWireframe);
			}
			else if (CULL_FRONT == pRS->cullMode)
			{
				if (nullptr == mpRasterStateFrontCullingWireFrame)
				{
					D3D11_RASTERIZER_DESC RasterizerDesc =
					{
						D3D11_FILL_WIREFRAME,   // D3D11_FILL_MODE FillMode;
						D3D11_CULL_FRONT,    // D3D11_CULL_MODE CullMode;
						TRUE,               // BOOL FrontCounterClockwise;
						0,                  // INT DepthBias;
						0,                  // FLOAT DepthBiasClamp;
						0,                  // FLOAT SlopeScaledDepthBias;
						FALSE,              // BOOL DepthClipEnable;
						FALSE,              // BOOL ScissorEnable;
						TRUE,               // BOOL MultisampleEnable;
						FALSE,              // BOOL AntialiasedLineEnable;
					};
					// Create the rasterizer state from the description we just filled out.
					HRESULT result = pDevice->CreateRasterizerState(&RasterizerDesc, &mpRasterStateFrontCullingWireFrame);
					if (FAILED(result))
					{
						return false;
					}
				}
				pContext->RSSetState(mpRasterStateFrontCullingWireFrame);
			}
			else
			{
				if (nullptr == mpRasterStateBackCullingWireFrame)
				{
					D3D11_RASTERIZER_DESC RasterizerDesc =
					{
						D3D11_FILL_WIREFRAME,   // D3D11_FILL_MODE FillMode;
						D3D11_CULL_BACK,    // D3D11_CULL_MODE CullMode;
						TRUE,               // BOOL FrontCounterClockwise;
						0,                  // INT DepthBias;
						0,                  // FLOAT DepthBiasClamp;
						0,                  // FLOAT SlopeScaledDepthBias;
						FALSE,              // BOOL DepthClipEnable;
						FALSE,              // BOOL ScissorEnable;
						TRUE,               // BOOL MultisampleEnable;
						FALSE,              // BOOL AntialiasedLineEnable;
					};
					// Create the rasterizer state from the description we just filled out.
					HRESULT result = pDevice->CreateRasterizerState(&RasterizerDesc, &mpRasterStateBackCullingWireFrame);
					if (FAILED(result))
					{
						return false;
					}
				}
				pContext->RSSetState(mpRasterStateBackCullingWireFrame);
			}
		}
		return true;
	}



	void CDX11RenderStateMgr::setDepthStencilState(ID3D11DeviceContext* pContext, SRenderState* pRS, int nStencilRef/*= 1*/)
	{
		mpCurrentDepthStencilState = pRS->pDepthStencilState;
		pContext->OMSetDepthStencilState(mpCurrentDepthStencilState, nStencilRef);
	}

	void CDX11RenderStateMgr::setBlendState(ID3D11DeviceContext* pContext, SRenderState* pRS, Vec4F blendFactor/*= {0, 0, 0, 0}*/, UINT SampleMas/*= 0xffffffff*/)
	{
		mpCurrentBlendingState = pRS->pBlendState;
		pContext->OMSetBlendState(mpCurrentBlendingState, blendFactor.data(), SampleMas);
	}

	void CDX11RenderStateMgr::setRasterizerState(ID3D11DeviceContext* pContext, SRenderState* pRS)
	{
		mpCurrentRasterState = pRS->pRasterizerState;
		pContext->RSSetState(pRS->pRasterizerState);
	}

	SRenderState* CDX11RenderStateMgr::createRenderState(SRenderStateDesc& renderStateDesc)
	{
		auto pDevice = mpDevice->getGraphicsDevice();
		SRenderState* pRenderState = WL_NEW(SRenderState, RenderState);
		HRESULT result = S_OK;
		if (renderStateDesc.bCreateDepthStencil)
		{
			D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
			memcpy(&depthStencilDesc, &renderStateDesc.depthStencilDesc, sizeof(SDepthStencilDesc));
			// Create the depth stencil state.
			ID3D11DepthStencilState* pDepthStencilState = nullptr;
			result = pDevice->CreateDepthStencilState(&depthStencilDesc, &pDepthStencilState);

			if (FAILED(result))
			{
				assert(false);
			}
			mDepthStencilStatesArray.push_back(pDepthStencilState);
			pRenderState->pDepthStencilState = pDepthStencilState;
		}
		if (renderStateDesc.bCreateBlend)
		{
			D3D11_BLEND_DESC blendStateDescription;
			memcpy(&blendStateDescription, &renderStateDesc.blendDesc, sizeof(SBlendDesc));
			ID3D11BlendState* pBlendState = nullptr;
			// Create the blend state using the description.
			result = pDevice->CreateBlendState(&blendStateDescription, &pBlendState);
			if (FAILED(result))
			{
				assert(false);
			}
			mBlendStateArray.push_back(pBlendState);
			pRenderState->pBlendState = pBlendState;
		}
		if (renderStateDesc.bCreateRasterizer)
		{
			D3D11_RASTERIZER_DESC RasterizerDesc;
			memcpy(&RasterizerDesc, &renderStateDesc.rasterizerDesc, sizeof(RasterizerDesc));
			// Create the rasterizer state from the description we just filled out.
			ID3D11RasterizerState* pRasterState;
			result = pDevice->CreateRasterizerState(&RasterizerDesc, &pRasterState);
			if (FAILED(result))
			{
				assert(false);
			}
			mRasterStatesArray.push_back(pRasterState);
			pRenderState->pRasterizerState = pRasterState;
		}
		return pRenderState;
	}

}