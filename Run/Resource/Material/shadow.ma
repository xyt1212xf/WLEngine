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
	vs = "Resource/Shader/shadow.vs",
	ps = "Resource/Shader/shadow.ps",
	vsMain = "vsMain",
	psMain = "psMain",
	--texture0 = "Resource/Texture/marble01.dds 0",
	
	AmbientColor = "1,1,1,1";
	DiffuseColor = "1,1,1,1";
	SpecularColor = "1,1,1,1";
	
	--默认的状态
	ZDepth = 0,
	ZWrite = 0,
	CullMode = 1,        --D3D11_CULL_NONE	= 1, D3D11_CULL_FRONT	= 2, D3D11_CULL_BACK	= 3
	Transparent = 0,
	Shadow = 1,
	FillMode = 3,  		--D3D11_FILL_WIREFRAME = 2, D3D11_FILL_SOLID==3
 	Topology = 4,		--TOPOLOGY_TRIANGLELIST
	EnableLight = 1, 	--是否有光照
	pass =
	{	
		[1] = 
		{
			ZDepth = 0,
			ZWrite = 0,
			CullMode = 1,        
			Transparent = 0,
			Shadow = 0,
	
			vsVersion = "vs_5_0",
			psVersion = "ps_5_0",
			
			vs = "Resource/Shader/shadow.vs",
			ps = "Resource/Shader/shadow.ps",
			vsMain = "vsMain",
			psMain = "psMain",		
			--texture0 = "Resource/Texture/grass_top.png 0",			
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
	
	psConstBuffer = 
	{
		[1] =
		{
			name = "lightViewMatrix",
			size = 64,
			registerIndex = 0
		},
		[2] =
		{
			name = "lightProjectionMatrix",
			size = 64,
			registerIndex = 0
		},
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
			SemanticName = "TEXCOORD",
			SemanticIndex = 0,
			Format = 16,							--DXGI_FORMAT_R32G32_FLOAT
			InputSlot = 0,
			AlignedByteOffset = 0xffffffff,		--D3D11_APPEND_ALIGNED_ELEMENT,
			InputSlotClass = 0, 				--D3D11_INPUT_PER_VERTEX_DATA
			InstanceDataStepRate = 0
		},
	}
}

function inputElementLayout(index)
	local t = material.elementItem[index]
	return t.SemanticName, t.SemanticIndex, t.Format, t.InputSlot, t.AlignedByteOffset, t.InputSlotClass, t.InstanceDataStepRate
end
