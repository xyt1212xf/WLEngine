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
	
	ZDepth = 1,
	CullMode = 3,        --D3D11_CULL_NONE	= 1, D3D11_CULL_FRONT	= 2, D3D11_CULL_BACK	= 3
	Transparent = 0,
	Shadow = 0,
	
	pass =
	{	
		[1] = 
		{
			vsVersion = "vs_4_0",
			gsVersion = "gs_4_0",
			vs = "Resource/Shader/streamOut.vs",
			gs = "Resource/Shader/streamOut.gs",
			vsMain = "vsMain",
			gsMain = "gsMain",
			InputDesc = 0,
			TopologyType = 1,					--D3D_PRIMITIVE_TOPOLOGY_POINTLIST
						typeBuffer = 
			{
				[1] = 
				{
					name = "bonesMatrices",
					size = 16320,
					registerIndex = 0
				},
			},
			
			--[[
			
			vsConstBuffer = 
			{
				[1] = 
				{
					name = "bonesMatrices",
					size = 16320,
					registerIndex = 1
				},
			},
			

			--[[
			textureBuffer = 
			{
				[1] = 
				{
					name = "bonesMatrices",
					size = 16320,
					registerIndex = 0
				},
			},

			textureConstBuffer = 
			{
				[1] = 
				{
					name = "bonesMatrices",
					size = 16320,
					registerIndex = 0
				},
			},
			--]]
		},
		[2] = 
		{
			vsVersion = "vs_5_0",
			psVersion = "ps_5_0",
			vs = "Resource/Shader/skin.vs",
			ps = "Resource/Shader/skin.ps",
			vsMain = "vsMain",
			psMain = "psMain",							
			InputDesc = 1,
			psConstBuffer = 
			{
				[1] = 
				{
					name = "cbUserChange",
					size = 32,
					registerIndex = 1
				},
			},
		},
	},
	
	elementItemCount = 6,
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
			SemanticName = "WEIGHTS",
			SemanticIndex = 0,
			Format = 28,						--DXGI_FORMAT_R8G8B8A8_UNORM
			InputSlot = 0,
			AlignedByteOffset = 32,				--D3D11_APPEND_ALIGNED_ELEMENT,
			InputSlotClass = 0, 				--D3D11_INPUT_PER_VERTEX_DATA
			InstanceDataStepRate = 0
		},
		[3] = 
		{
			SemanticName = "BONES",
			SemanticIndex = 0,
			Format = 30,						--DXGI_FORMAT_R8G8B8A8_UINT
			InputSlot = 0,
			AlignedByteOffset = 0xffffffff,		--D3D11_APPEND_ALIGNED_ELEMENT,
			InputSlotClass = 0, 				--D3D11_INPUT_PER_VERTEX_DATA
			InstanceDataStepRate = 0
		},
		[4] = 
		{
			SemanticName = "NORMAL",
			SemanticIndex = 0,
			Format = 6,							--DXGI_FORMAT_R32G32B32_FLOAT
			InputSlot = 0,
			AlignedByteOffset = 0xffffffff,		--D3D11_APPEND_ALIGNED_ELEMENT,
			InputSlotClass = 0, 				--D3D11_INPUT_PER_VERTEX_DATA
			InstanceDataStepRate = 0
		},
		[5] = 
		{
			SemanticName = "TEXCOORD",
			SemanticIndex = 0,
			Format = 16,						--DXGI_FORMAT_R32G32_FLOAT
			InputSlot = 0,
			AlignedByteOffset = 32,				--D3D11_APPEND_ALIGNED_ELEMENT,
			InputSlotClass = 0, 				--D3D11_INPUT_PER_VERTEX_DATA
			InstanceDataStepRate = 0
		},
		[6] = 
		{
			SemanticName = "TANGENT",
			SemanticIndex = 0,
			Format = 6,							--DXGI_FORMAT_R32G32B32_FLOAT
			InputSlot = 0,
			AlignedByteOffset = 32,				--D3D11_APPEND_ALIGNED_ELEMENT,
			InputSlotClass = 0, 				--D3D11_INPUT_PER_VERTEX_DATA
			InstanceDataStepRate = 0
		},
	},
	
	elementItemCountOne = 4,
	elementItemOne = 
	{
		[1] = 
		{
			SemanticName = "POSITION",
			SemanticIndex = 0,
			Format = 6, 						-- DXGI_FORMAT_R32G32B32A32_FLOAT
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
			Format = 16,						--DXGI_FORMAT_R32G32_FLOAT
			InputSlot = 0,
			AlignedByteOffset = 32,				--D3D11_APPEND_ALIGNED_ELEMENT,
			InputSlotClass = 0, 				--D3D11_INPUT_PER_VERTEX_DATA
			InstanceDataStepRate = 0
		},
		[4] = 
		{
			SemanticName = "TANGENT",
			SemanticIndex = 0,
			Format = 6,							--DXGI_FORMAT_R32G32B32_FLOAT
			InputSlot = 0,
			AlignedByteOffset = 32,				--D3D11_APPEND_ALIGNED_ELEMENT,
			InputSlotClass = 0, 				--D3D11_INPUT_PER_VERTEX_DATA
			InstanceDataStepRate = 0
		},
	}
}

function inputElementLayout(index)
	local t = material.elementItem[index]
	return t.SemanticName, t.SemanticIndex, t.Format, t.InputSlot, t.AlignedByteOffset, t.InputSlotClass, t.InstanceDataStepRate
end


function inputElementLayoutOne(index)
	local t = material.elementItemOne[index]
	return t.SemanticName, t.SemanticIndex, t.Format, t.InputSlot, t.AlignedByteOffset, t.InputSlotClass, t.InstanceDataStepRate
end