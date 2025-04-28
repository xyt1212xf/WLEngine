textureSampler0 = 
{
	Filter = 0x15, --D3D11_FILTER_MIN_MAG_MIP_LINEAR ,
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
	vsVersion = "vs_5_0",
	psVersion = "ps_5_0",
	vs = "Resource/Shader/depth.vs",
	ps = "Resource/Shader/depth.ps",
	vsMain = "vsShader",
	psMain = "psShader",
	
	--默认的状态
	ZDepth = 1,
	ZWrite = 1,
	CullMode = 1, --D3D11_CULL_NONE	= 1, D3D11_CULL_FRONT = 2, D3D11_CULL_BACK = 3
	Transparent = 0,
	Shadow = 0,
	FillMode = 3, --D3D11_FILL_WIREFRAME = 2, D3D11_FILL_SOLID == 3
	Topology = 4, --TOPOLOGY_TRIANGLELIST
	EnableLight = 0, --是否有光照

	pass =
	{	
		[1] = 
		{
			ZDepth = 1,
			ZWrite = 1,			
			CullMode = 1,        
			Transparent = 0,
			Shadow = 0,
	
			vsVersion = "vs_5_0",
			psVersion = "ps_5_0",
			
			vs = "Resource/Shader/depth.vs",
			ps = "Resource/Shader/depth.ps",
			vsMain = "vsShader",
			psMain = "psShader",	
		},
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
			name = "VPMatrix",
			size = 64,
			registerIndex = 0,
			packoffset = 0,
		},
		[3] = 
		{
			name = "lightViewMatrix",
			size = 64,
			registerIndex = 0,
			packoffset = 0,
		},
		[4] = 
		{
			name = "lightProjectionMatrix",
			size = 64,
			registerIndex = 0, 
			packoffset = 0,
		},
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
