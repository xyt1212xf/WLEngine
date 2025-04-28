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
	hsVersion = "hs_5_0",
	dsVersion = "ds_5_0",
	psVersion = "ps_5_0",

	vs = "Resource/Shader/Tessellation.vs",
	hs = "Resource/Shader/Tessellation.hs",
	ds = "Resource/Shader/Tessellation.ds",
	ps = "Resource/Shader/Tessellation.ps",
	vsMain = "vsMain",
	hsMain = "hsMain",
	dsMain = "dsMain",
	psMain = "psMain",
	
	ZDepth = 1,
	FillCullMode = 2,   --D3D11_CULL_NONE = 1, D3D11_CULL_FRONT	= 2, D3D11_CULL_BACK = 3 D3D11_CULL_SOLID_NONE = 4, D3D11_CULL_SOLID_FRONT	= 5, D3D11_SOLID_CULL_BACK = 6
	Transparent = 0,
	Topology =  35, 	--D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST
	
	vsConstBuffer = 
	{
	},
	
	hsConstBuffer = 
	{
		[1] = 
		{
			name = "TessellationBuffer",
			size = 16,
			registerIndex = 9
		},
	},
	
	dsConstBuffer = 
	{
		[1] = 
		{
			name = "MatrixBuffer",
			size = 192,
			registerIndex = 0
		},
	},
	
	psConstBuffer = 
	{
	},
	
	elementItemCount = 2,
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
			SemanticName = "COLOR",
			SemanticIndex = 0,
			Format = 2,							--DXGI_FORMAT_R32G32B32A32_FLOAT
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
