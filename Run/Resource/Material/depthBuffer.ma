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
}

material = {}
material =
{
	vsVersion = "vs_5_0",
	psVersion = "ps_5_0",
	vs = "Resource/Shader/depthBufferEffect.vs",
	ps = "Resource/Shader/depthBufferEffect.ps",
	vsMain = "vsMain",
	psMain = "psMain",
	
	ZDepth = 1,
	CullMode = 1,        --D3D11_CULL_NONE	= 1, D3D11_CULL_FRONT	= 2, D3D11_CULL_BACK	= 3
	Transparent = 0,
	
	vsConstBuffer = 
	{
	},
	
	psConstBuffer = 
	{
	},
	
	elementItemCount = 1,
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
	}
}

function inputElementLayout(index)
	local t = material.elementItem[index]
	return t.SemanticName, t.SemanticIndex, t.Format, t.InputSlot, t.AlignedByteOffset, t.InputSlotClass, t.InstanceDataStepRate
end
