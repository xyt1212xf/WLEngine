textureSampler0 = 
{
	Filter = 0x0, --D3D11_FILTER_MIN_MAG_MIP_LINEAR,
    AddressU = 3, --D3D11_TEXTURE_ADDRESS_CLAMP,
    AddressV = 3, --D3D11_TEXTURE_ADDRESS_CLAMP,
    AddressW = 3, --D3D11_TEXTURE_ADDRESS_CLAMP,
    MipLODBias = 0.0,
    MaxAnisotropy = 1,
    ComparisonFunc = 8, --D3D11_COMPARISON_ALWAYS,
    MinLOD = 0,
    MaxLOD = 0, --D3D11_FLOAT32_MAX,
}

material = {}
material =
{
	ZDepth = 1,
	CullMode = 1,        --D3D11_CULL_NONE	= 1, D3D11_CULL_FRONT	= 2, D3D11_CULL_BACK	= 3
	Transparent = 0,
	Shadow = 0,
	pass =
	{	
		[1] = 
		{
			vsVersion = "vs_5_0",
			psVersion = "ps_5_0",
			vs = "Resource/Shader/PostEffect.vs",
			ps = "Resource/Shader/PostEffect.ps",
			vsMain = "vsShader",
			psMain = "psShader",
			InputDesc = 0,
			vsConstBuffer = 
			{
			},
		},
	},
	
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
		},
		[4] = 
		{
			SemanticName = "TANGENT",
			SemanticIndex = 0,
			Format = 6,							--DXGI_FORMAT_R32G32B32_FLOAT
			InputSlot = 0,
			AlignedByteOffset = 0xffffffff,		--D3D11_APPEND_ALIGNED_ELEMENT,
			InputSlotClass = 0, 				--D3D11_INPUT_PER_VERTEX_DATA
			InstanceDataStepRate = 0
		},
		[5] = 
		{
			SemanticName = "BINORMAL",
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
