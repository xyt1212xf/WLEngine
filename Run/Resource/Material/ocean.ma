textureSampler0 = 
{
	Filter = 0, --D3D11_FILTER_MIN_MAG_MIP_POINT,
    AddressU = 1, --D3D11_TEXTURE_ADDRESS_WRAP,
    AddressV = 1, --D3D11_TEXTURE_ADDRESS_WRAP,
    AddressW = 1, --D3D11_TEXTURE_ADDRESS_WRAP,
    MipLODBias = 0.0,
    MaxAnisotropy = 1,
    ComparisonFunc = 1, --D3D11_COMPARISON_NEVER,
	BorderColor = "1,1,1,1",
    MinLOD = 0,
    MaxLOD = 0, --D3D11_FLOAT32_MAX,
}


textureSampler1 = 
{
	Filter = 0, --D3D11_FILTER_MIN_MAG_MIP_POINT,
    AddressU = 1, --D3D11_TEXTURE_ADDRESS_WRAP,
    AddressV = 1, --D3D11_TEXTURE_ADDRESS_WRAP,
    AddressW = 1, --D3D11_TEXTURE_ADDRESS_WRAP,
    MipLODBias = 0.0,
    MaxAnisotropy = 1,
    ComparisonFunc = 1, --D3D11_COMPARISON_NEVER,
	BorderColor = "1,1,1,1",
    MinLOD = 0,
    MaxLOD = 0, --D3D11_FLOAT32_MAX,
}


textureSampler2 = 
{
	Filter = 0x55, --D3D11_FILTER_ANISOTROPIC,
    AddressU = 1, --D3D11_TEXTURE_ADDRESS_WRAP,
    AddressV = 1, --D3D11_TEXTURE_ADDRESS_WRAP,
    AddressW = 1, --D3D11_TEXTURE_ADDRESS_WRAP,
    MipLODBias = 0.0,
    MaxAnisotropy = 8,
    ComparisonFunc = 1, --D3D11_COMPARISON_NEVER,
	BorderColor = "1,1,1,1",
    MinLOD = 0,
    MaxLOD = 0, --D3D11_FLOAT32_MAX,
}

textureSampler3 = 
{
	Filter = 0x15, --D3D11_FILTER_MIN_MAG_MIP_LINEAR,
    AddressU = 3, --D3D11_TEXTURE_ADDRESS_CLAMP,
    AddressV = 3, --D3D11_TEXTURE_ADDRESS_CLAMP,
    AddressW = 3, --D3D11_TEXTURE_ADDRESS_CLAMP,
    MipLODBias = 0.0,
    MaxAnisotropy = 8,
    ComparisonFunc = 1, --D3D11_COMPARISON_NEVER,
	BorderColor = "1,1,1,1",
    MinLOD = 0,
    MaxLOD = 0, --D3D11_FLOAT32_MAX,
}


material = {}
material =
{
	--csVersion = "cs_5_0",
	--cs = "Resource/Shader/fft_512x512_c2c.hlsl",
	--csMain = "Radix008A_CS",

	--默认的状态
	ZDepth = 1,
	ZCompare = 2,		 --D3D11_COMPARISON_LESS
	ZWrite = 1,			 --D3D11_DEPTH_WRITE_MASK_ALL
	CullMode = 1,        --D3D11_CULL_NONE	= 1, D3D11_CULL_FRONT	= 2, D3D11_CULL_BACK	= 3
	Transparent = 0,
	Shadow = 0,
	FillMode = 3,  		--D3D11_FILL_WIREFRAME = 2, D3D11_FILL_SOLID==3
 	Topology = 4,		--TOPOLOGY_TRIANGLELIST
	Stencil = 0,
	pass =
	{	
		[1] = 
		{
			ZDepth = 1,
			ZWrite = 1,
			CullMode = 1,        
			Transparent = 0,
			Shadow = 0,
	
			vsVersion = "vs_4_0",
			psVersion = "ps_4_0",
			
			vs = "Resource/Shader/oceanShading.hlsl",
			ps = "Resource/Shader/oceanShading.hlsl",
			--cs = "Resource/Shader/oceanShading.hlsl",
			vsMain = "OceanSurfVS",
			psMain = "OceanSurfPS",	
			--csMain = "OceanSurfCS",	
		},
		--[[
		[2] = 
		{
			ZCompare = 1,
			ZDepth = 1,
			CullMode = 1,        
			Transparent = 0,
			Shadow = 0,
	
			csVersion = "cs_4_0",
			cs = "Resource/Shader/fft_512x512_c2c.hlsl",
			csMain = "Radix008A_CS",			
		},
		[3] = 
		{
			ZCompare = 1,
			ZDepth = 1,
			CullMode = 1,        
			Transparent = 0,
			Shadow = 0,
	
			csVersion = "cs_4_0",			
			cs = "Resource/Shader/fft_512x512_c2c.hlsl",
			csMain = "Radix008A_CS2",			
		},
		[4] = 
		{
			ZCompare = 1,
			ZDepth = 1,
			CullMode = 1,        
			Transparent = 0,
			Shadow = 0,
	
			csVersion = "cs_4_0",			
			cs = "Resource/Shader/oceanSimulatorCs.hlsl",
			csMain = "UpdateSpectrumCS",			
		},
		
		[5] = 
		{
			ZCompare = 1,
			ZDepth = 1,
			CullMode = 1,        
			Transparent = 0,
			Shadow = 0,
	
			vsVersion = "vs_4_0",			
			vs = "Resource/Shader/oceanVsPs.hlsl",
			vsMain = "QuadVS",			
		},
		
		[6] = 
		{
			ZCompare = 1,
			ZDepth = 1,
			CullMode = 1,        
			Transparent = 0,
			Shadow = 0,
	
			psVersion = "ps_4_0",			
			ps = "Resource/Shader/oceanVsPs.hlsl",
			psMain = "UpdateDisplacementPS",			
		},
		
		[7] = 
		{
			ZCompare = 1,
			ZDepth = 1,
			CullMode = 1,        
			Transparent = 0,
			Shadow = 0,
	
			psVersion = "ps_4_0",			
			ps = "Resource/Shader/oceanVsPs.hlsl",
			psMain = "GenGradientFoldingPS",			
		},
		]]
	},
	
	vsConstBuffer = 
	{
		[1] = 
		{
			name = "WVPMatrix",
			size = 192,
			registerIndex = 0
		},

		[2] = 
		{
			name = "ShadingData",
			size = 144,
			registerIndex = 2
		},
		
		[3] = 
		{
			name = "PerCallData",
			size = 160,
			registerIndex = 4
		},
	},

	
	psConstBuffer = 
	{
		[1] = 
		{
			name = "ShadingData",
			size = 144,
			registerIndex = 2
		},
		
		[2] = 
		{
			name = "PerCallData",
			size = 160,
			registerIndex = 4
		},
	},
	
	--[[
	csConstBuffer = 
	{	
		[1] = 
		{
			name = "PerFrameCB",
			size = 16,
			registerIndex = 1,
			usage = 2,
			bindFlags = 4,
			cpuAccessFlags = 0x10000,
		},
	},
	--]]
	textureBuffer = 
	{	
	--[[
		[1] = 
		{
			name = "displacement",
			textureType = 2,
			registerIndex = 0,
			width = 512,
			height = 512,
			format = 2,
			sampler = 0,
			usage = 0,
			bindFlags = 40,
			cpuAccessFlags = 0,
			shaderFlags = 1;
		},
		--]]
		[1] = 
		{
			name = "perlinNoise",
			textureType = 2,
			registerIndex = 1,
			width = 64,
			height = 64,
			format = 10,
			path = "Resource/Texture/perlinNoise.dds",
			sampler = 1,
			usage = 0,
			bindFlags = 0x8,
			cpuAccessFlags = 0,
			shaderFlags= 3;
		},
		
		[2] = 
		{
			name = "gradient",
			textureType = 2,
			registerIndex = 2,
			width = 512,
			height = 512,
			format = 10,
			sampler = 2,
			usage = 0,
			bindFlags = 40,
			cpuAccessFlags = 0,
			shaderFlags = 2;
		},
	},
	
	elementItemCount = 1,
	elementItem = 
	{
		[1] = 
		{
			SemanticName = "POSITION",
			SemanticIndex = 0,
			Format = 16, 						-- DXGI_FORMAT_R32G32_FLOAT
			InputSlot = 0,
			AlignedByteOffset = 0,
			InputSlotClass = 0, 				--D3D11_INPUT_PER_VERTEX_DATA
			InstanceDataStepRate = 0
		},
	}
}

function inputElementLayout(index)
	local t = material.elementItem[index]
	return t.SemanticName, t.SemanticIndex, t.Format, t.InputSlot, t.AlignedByteOffset, t.InputSlotClass, t.InstanceDataStepRate
end
