textureSampler0 = 
{
	Filter = 0x15, --D3D11_FILTER_MIN_MAG_MIP_LINEAR ,
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
	vs = "Resource/Shader/deferred.vs",
	ps = "Resource/Shader/deferred.ps",
	vsMain = "vsShader",
	psMain = "psShader",
	
	--默认的状态
	Stencil = 0,
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
			
			vs = "Resource/Shader/deferred.vs",
			ps = "Resource/Shader/deferred.ps",
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
			SemanticName = "NORMAL",
			SemanticIndex = 0,
			Format = 6,							--DXGI_FORMAT_R32G32B32_FLOAT
			InputSlot = 0,
			AlignedByteOffset = 0xffffffff,		--D3D11_APPEND_ALIGNED_ELEMENT,
			InputSlotClass = 0, 				--D3D11_INPUT_PER_VERTEX_DATA
			InstanceDataStepRate = 0
		},
		[3] = 
		{
			SemanticName = "TEXCOORD",
			SemanticIndex = 0,
			Format = 16,							--DXGI_FORMAT_R32G32_FLOAT
			InputSlot = 0,
			AlignedByteOffset = 0xffffffff,		--D3D11_APPEND_ALIGNED_ELEMENT,
			InputSlotClass = 0, 				--D3D11_INPUT_PER_VERTEX_DATA
			InstanceDataStepRate = 0
		},

		[4] = 
		{
			SemanticName = "COLOR",
			SemanticIndex = 0,
			Format = 41,						--DXGI_FORMAT_R32_FLOAT
			InputSlot = 0,
			AlignedByteOffset = 0xffffffff,		--D3D11_APPEND_ALIGNED_ELEMENT,
			InputSlotClass = 0, 				--D3D11_INPUT_PER_VERTEX_DATA
			InstanceDataStepRate = 0
		},
		[5] = 
		{
			SemanticName = "COLOR",
			SemanticIndex = 1,
			Format = 41,						--DXGI_FORMAT_R32_FLOAT
			InputSlot = 0,
			AlignedByteOffset = 0xffffffff,		--D3D11_APPEND_ALIGNED_ELEMENT,
			InputSlotClass = 0, 				--D3D11_INPUT_PER_VERTEX_DATA
			InstanceDataStepRate = 0
		},
		[6] = 
		{
			SemanticName = "TANGENT",
			SemanticIndex = 0,
			Format = 6,							--DXGI_FORMAT_R32G32B32_FLOAT
			InputSlot = 0,
			AlignedByteOffset = 0xffffffff,		--D3D11_APPEND_ALIGNED_ELEMENT,
			InputSlotClass = 0, 				--D3D11_INPUT_PER_VERTEX_DATA
			InstanceDataStepRate = 0
		},
		[7] = 
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
