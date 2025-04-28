textureSampler0 = 
{
	Filter = 0x15, --D3D11_FILTER_MIN_MAG_MIP_LINEAR,
    AddressU = 1, --D3D11_TEXTURE_ADDRESS_WRAP,
    AddressV = 1, --D3D11_TEXTURE_ADDRESS_WRAP,
    AddressW = 1, --D3D11_TEXTURE_ADDRESS_WRAP,
    MipLODBias = 0.0,
    MaxAnisotropy = 1,
    ComparisonFunc = 8, --D3D11_COMPARISON_ALWAYS,
    MinLOD = 0,
    MaxLOD = 0, --D3D11_FLOAT32_MAX,
	BorderColor = "0,0,0,0",
}

textureSampler1 = 
{
	Filter = 0x15, --D3D11_FILTER_MIN_MAG_MIP_LINEAR,
    AddressU = 1, --D3D11_TEXTURE_ADDRESS_WRAP,
    AddressV = 1, --D3D11_TEXTURE_ADDRESS_WRAP,
    AddressW = 1, --D3D11_TEXTURE_ADDRESS_WRAP,
    MipLODBias = 0.0,
    MaxAnisotropy = 1,
    ComparisonFunc = 8, --D3D11_COMPARISON_ALWAYS,
    MinLOD = 0,
    MaxLOD = 0, --D3D11_FLOAT32_MAX,
	BorderColor = "0,0,0,0",
}

textureSampler2 = 
{
	Filter = 0x0, --D3D11_FILTER_MIN_MAG_MIP_POINT,
    AddressU = 3, --D3D11_TEXTURE_ADDRESS_CLAMP,
    AddressV = 3, --D3D11_TEXTURE_ADDRESS_CLAMP,
    AddressW = 3, --D3D11_TEXTURE_ADDRESS_CLAMP,
    MipLODBias = 0.0,
    MaxAnisotropy = 1,
    ComparisonFunc = 8, --D3D11_COMPARISON_ALWAYS,
    MinLOD = 0,
    MaxLOD = 0, --D3D11_FLOAT32_MAX,
	BorderColor = "0,0,0,0",
}

material = {}
material =
{
	pass =
	{	
		[1] = 
		{
			vsVersion = "vs_5_0",
			psVersion = "ps_5_0",
			
			vs = "Resource/Shader/deferred.vs",
			ps = "Resource/Shader/deferred.ps",
			vsMain = "vsMain",
			psMain = "psMain",
					
		},
		[2] = 
		{		
			vsVersion = "vs_5_0",
			psVersion = "ps_5_0",
	
			vs = "Resource/Shader/deferredLight.vs",
			ps = "Resource/Shader/deferredLight.ps",
			vsMain = "vsMain",
			psMain = "psMain",
			
			vsConstBuffer = 
			{
				[1] = 
				{
					name = "LightBuffer2",
					size = 16,
					registerIndex = 1
				},
				[2] = 
				{
					name = "LightMatrixBuffer",
					size = 128,
					registerIndex = 2
				},
			},
	
			psConstBuffer = 
			{
				[1] = 
				{
					name = "LightBuffer",
					size = 16,
					registerIndex = 1
				},
			},
		},
	},
	
	ZDepth = 1,
	CullMode = 3,        --D3D11_CULL_NONE	= 1, D3D11_CULL_FRONT	= 2, D3D11_CULL_BACK	= 3
	Transparent = 0,
	
	elementItemCount = 3,
	elementItem = 
	{
		[1] = 
		{
			SemanticName = "POSITION",
			SemanticIndex = 0,
			Format = 6, 						-- DXGI_FORMAT_R32G32B32_FLOAT
			InputSlot = 0,
			AlignedByteOffset = 0,
			InputSlotClass = 0, 				--D3D11_INPUT_PER_VERTEX_DATA
			InstanceDataStepRate = 0
		},
		[2] = 
		{
			SemanticName = "TEXCOORD",
			SemanticIndex = 0,
			Format = 16,							--DXGI_FORMAT_R32G32_FLOAT
			InputSlot = 0,
			AlignedByteOffset = 0xffffffff,		--D3D11_APPEND_ALIGNED_ELEMENT,
			InputSlotClass = 0, 				--D3D11_INPUT_PER_VERTEX_DATA
			InstanceDataStepRate = 0
		},
		[3] = 
		{
			SemanticName = "NORMAL",
			SemanticIndex = 0,
			Format = 6,							--DXGI_FORMAT_R32G32B32_FLOAT
			InputSlot = 0,
			AlignedByteOffset = 0xffffffff,		--D3D11_APPEND_ALIGNED_ELEMENT,
			InputSlotClass = 0, 				--D3D11_INPUT_PER_VERTEX_DATA
			InstanceDataStepRate = 0
		}
	}
}

function inputElementLayout(index)
	local t = material.elementItem[index]
	return t.SemanticName, t.SemanticIndex, t.Format, t.InputSlot, t.AlignedByteOffset, t.InputSlotClass, t.InstanceDataStepRate
end
