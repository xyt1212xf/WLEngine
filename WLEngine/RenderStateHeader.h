#pragma once
#include "Common.h"

namespace WL
{
#define  DEFAULT_STENCIL_WRITE_MASK 0xff

	enum StateFlag
	{
		DepthStencilOn =  1 << 0,
		DepthStencilOff = 1 << 1,
		AlphaOn = 1 << 2,
		AlphaOff = 1 << 3,
		ZBufferOn = 1 << 4,
		ZBufferOff = 1 << 5,
		FrontCull = 1 << 6,
		BackCull = 1 << 7,
		NoCull = 1 << 8,
		Fill_WireFrame = 1 << 9,
		Fill_Solid = 1 << 10,
	};
	enum CULL_MODE
	{
		CULL_NONE = 1,
		CULL_FRONT = 2,
		CULL_BACK = 3
	};

	enum FILL_MODE
	{
		FILL_WIREFRAME = 2,
		FILL_SOLID = 3
	};

	enum STENCIL_OP
	{
		STENCIL_OP_KEEP = 1,
		STENCIL_OP_ZERO = 2,
		STENCIL_OP_REPLACE = 3,
		STENCIL_OP_INCR_SAT = 4,
		STENCIL_OP_DECR_SAT = 5,
		STENCIL_OP_INVERT = 6,
		STENCIL_OP_INCR = 7,
		STENCIL_OP_DECR = 8,
	};

	enum COMPARISON_FUNC
	{
		COMPARISON_NEVER = 1,
		COMPARISON_LESS = 2,
		COMPARISON_EQUAL = 3,
		COMPARISON_LESS_EQUAL = 4,
		COMPARISON_GREATER = 5,
		COMPARISON_NOT_EQUAL = 6,
		COMPARISON_GREATER_EQUAL = 7,
		COMPARISON_ALWAYS = 8,
	};

	enum FILTER
	{
		FILTER_MIN_MAG_MIP_POINT = 0,
		FILTER_MIN_MAG_POINT_MIP_LINEAR = 0x1,
		FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT = 0x4,
		FILTER_MIN_POINT_MAG_MIP_LINEAR = 0x5,
		FILTER_MIN_LINEAR_MAG_MIP_POINT = 0x10,
		FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR = 0x11,
		FILTER_MIN_MAG_LINEAR_MIP_POINT = 0x14,
		FILTER_MIN_MAG_MIP_LINEAR = 0x15,
		FILTER_ANISOTROPIC = 0x55,
		FILTER_COMPARISON_MIN_MAG_MIP_POINT = 0x80,
		FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR = 0x81,
		FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT = 0x84,
		FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR = 0x85,
		FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT = 0x90,
		FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR = 0x91,
		FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT = 0x94,
		FILTER_COMPARISON_MIN_MAG_MIP_LINEAR = 0x95,
		FILTER_COMPARISON_ANISOTROPIC = 0xd5,
		FILTER_MINIMUM_MIN_MAG_MIP_POINT = 0x100,
		FILTER_MINIMUM_MIN_MAG_POINT_MIP_LINEAR = 0x101,
		FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT = 0x104,
		FILTER_MINIMUM_MIN_POINT_MAG_MIP_LINEAR = 0x105,
		FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT = 0x110,
		FILTER_MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR = 0x111,
		FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT = 0x114,
		FILTER_MINIMUM_MIN_MAG_MIP_LINEAR = 0x115,
		FILTER_MINIMUM_ANISOTROPIC = 0x155,
		FILTER_MAXIMUM_MIN_MAG_MIP_POINT = 0x180,
		FILTER_MAXIMUM_MIN_MAG_POINT_MIP_LINEAR = 0x181,
		FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT = 0x184,
		FILTER_MAXIMUM_MIN_POINT_MAG_MIP_LINEAR = 0x185,
		FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT = 0x190,
		FILTER_MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR = 0x191,
		FILTER_MAXIMUM_MIN_MAG_LINEAR_MIP_POINT = 0x194,
		FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR = 0x195,
		FILTER_MAXIMUM_ANISOTROPIC = 0x1d5
	};

	enum DEPTH_WRITE_MASK
	{
		DEPTH_WRITE_MASK_ZERO = 0,
		DEPTH_WRITE_MASK_ALL = 1,
	};
	enum TEXTURE_ADDRESS_MODE
	{
		TEXTURE_ADDRESS_WRAP = 1,
		TEXTURE_ADDRESS_MIRROR = 2,
		TEXTURE_ADDRESS_CLAMP = 3,
		TEXTURE_ADDRESS_BORDER = 4,
		TEXTURE_ADDRESS_MIRROR_ONCE = 5
	};

	enum BLEND
	{
		BLEND_ZERO = 1,
		BLEND_ONE = 2,
		BLEND_SRC_COLOR = 3,
		BLEND_INV_SRC_COLOR = 4,
		BLEND_SRC_ALPHA = 5,
		BLEND_INV_SRC_ALPHA = 6,
		BLEND_DEST_ALPHA = 7,
		BLEND_INV_DEST_ALPHA = 8,
		BLEND_DEST_COLOR = 9,
		BLEND_INV_DEST_COLOR = 10,
		BLEND_SRC_ALPHA_SAT = 11,
		BLEND_BLEND_FACTOR = 14,
		BLEND_INV_BLEND_FACTOR = 15,
		BLEND_SRC1_COLOR = 16,
		BLEND_INV_SRC1_COLOR = 17,
		BLEND_SRC1_ALPHA = 18,
		BLEND_INV_SRC1_ALPHA = 19
	};

	enum BLEND_OP
	{
		BLEND_OP_ADD = 1,
		BLEND_OP_SUBTRACT = 2,
		BLEND_OP_REV_SUBTRACT = 3,
		BLEND_OP_MIN = 4,
		BLEND_OP_MAX = 5
	};

	enum COLOR_WRITE_ENABLE
	{
		COLOR_WRITE_ENABLE_RED = 1,
		COLOR_WRITE_ENABLE_GREEN = 2,
		COLOR_WRITE_ENABLE_BLUE = 4,
		COLOR_WRITE_ENABLE_ALPHA = 8,
		COLOR_WRITE_ENABLE_ALL = (((COLOR_WRITE_ENABLE_RED | COLOR_WRITE_ENABLE_GREEN) | COLOR_WRITE_ENABLE_BLUE) | COLOR_WRITE_ENABLE_ALPHA),
	};

	struct RenderTargetBlendDesc
	{
		BOOL BlendEnable = FALSE;
		BLEND SrcBlend = BLEND_ONE;
		BLEND DestBlend = BLEND_ZERO;
		BLEND_OP BlendOp = BLEND_OP_ADD;
		BLEND SrcBlendAlpha = BLEND_ONE;
		BLEND DestBlendAlpha = BLEND_ZERO;
		BLEND_OP BlendOpAlpha = BLEND_OP_ADD;
		UINT8 RenderTargetWriteMask = COLOR_WRITE_ENABLE_ALL;
	};

	struct SamplerDesc
	{
		FILTER Filter = FILTER_MIN_MAG_MIP_LINEAR;
		TEXTURE_ADDRESS_MODE AddressU = TEXTURE_ADDRESS_CLAMP;
		TEXTURE_ADDRESS_MODE AddressV = TEXTURE_ADDRESS_CLAMP;
		TEXTURE_ADDRESS_MODE AddressW = TEXTURE_ADDRESS_CLAMP;
		float MipLODBias = 0;
		UINT MaxAnisotropy = 16;
		COMPARISON_FUNC ComparisonFunc = COMPARISON_NEVER;
		float BorderColor[4] = {0, 0, 0, 0};
		float MinLOD = -FLT_MAX;
		float MaxLOD = FLT_MAX;
	};

	struct DepthStencilOpDesc
	{
		STENCIL_OP StencilFailOp = STENCIL_OP_KEEP;
		STENCIL_OP StencilDepthFailOp = STENCIL_OP_KEEP;
		STENCIL_OP StencilPassOp = STENCIL_OP_KEEP;
		COMPARISON_FUNC StencilFunc = COMPARISON_ALWAYS;
	};
	
	struct DepthStencilDesc
	{
		BOOL DepthEnable = TRUE;
		DEPTH_WRITE_MASK DepthWriteMask = DEPTH_WRITE_MASK_ALL;
		COMPARISON_FUNC DepthFunc = COMPARISON_LESS;
		BOOL StencilEnable = FALSE;
		UINT8 StencilReadMask = DEFAULT_STENCIL_WRITE_MASK;
		UINT8 StencilWriteMask = DEFAULT_STENCIL_WRITE_MASK;
		DepthStencilOpDesc FrontFace;
		DepthStencilOpDesc BackFace;
	};
	
	struct BlendDesc
	{
		BOOL AlphaToCoverageEnable = FALSE;
		BOOL IndependentBlendEnable = FALSE;
		RenderTargetBlendDesc RenderTarget[8];
	};

	struct RasterizerDesc
	{
		FILL_MODE FillMode = FILL_SOLID;
		CULL_MODE CullMode = CULL_BACK;
		BOOL FrontCounterClockwise = FALSE;
		INT32 DepthBias = 0;
		float DepthBiasClamp = 0.0f;
		float SlopeScaledDepthBias = 0.0f;
		BOOL DepthClipEnable = FALSE;
		BOOL ScissorEnable = FALSE;
		BOOL MultisampleEnable = FALSE;
		BOOL AntialiasedLineEnable = FALSE;
	};

	struct RenderStateDesc
	{
		//data
		int fillMode = 3;
		int cullMode = 1;
		int depthWrite = 1;
		int depthCompare = 1;
		
		bool transparent = false;
		bool depth = true;
		bool stencil = false;
		bool shadow = false;
		bool enableLight = false;
		bool bCreateDepthStencil = true;
		bool bCreateRasterizer = true;
		bool bCreateBlend = true;
		DepthStencilDesc depthStencilDesc;
		RasterizerDesc   rasterizerDesc;
		BlendDesc		 blendDesc;
		//function
		void reset()
		{
			fillMode = 3;
			cullMode = 1;
			depthWrite = 1;
			transparent = false;
			depth = true;
			stencil = false;
			shadow = false;
			enableLight = false;
			
			memset(&depthStencilDesc, 0, sizeof(DepthStencilDesc)); 
			memset(&rasterizerDesc, 0, sizeof(RasterizerDesc));
			memset(&blendDesc, 0, sizeof(BlendDesc));
		}
	};
}