
#define MAX_BONE_MATRICES 255
//////////////////////
// CONSTANT BUFFERS //
//////////////////////
cbuffer MatrixBuffer: register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};


// Constant buffer for bone matrices
cbuffer bonesMatrices : register(b1)
{
    matrix g_mConstBoneWorld[MAX_BONE_MATRICES];
};



// TBuffer for bone matrices
tbuffer tbAnimMatrices : register(t0)
{
    matrix g_mTexBoneWorld[MAX_BONE_MATRICES];
};

Texture1D g_txTexBoneWorld : register(t0);

//--------------------------------------------------------------------------------------
// Buffers (this is the buffer object for bone matrices)
//--------------------------------------------------------------------------------------
Buffer<float4> g_BufferBoneWorld : register(t0);

//--------------------------------------------------------------------------------------
// Helper struct for passing back skinned vertex information
//--------------------------------------------------------------------------------------
struct SkinnedInfo
{
    float4 position;
    float3 normal;
    float3 tangent;
};


//////////////
// TYPEDEFS //
//////////////
struct VSSkinnedIn
{
    float3 position : POSITION;
    float4 weights : WEIGHTS;
    uint4  bones : BONES;			//Bone indices	
	float3 normal : NORMAL;
	float2 tex : TEXCOORD0;
	float3 tangent : TANGENT0;
};



struct VSStreamOut
{
    float4 Pos	: POSITION;			//Position
    float3 Norm : NORMAL;			//Normal
    float2 Tex	: TEXCOORD;		    //Texture coordinate
    float3 Tangent : TANGENT;		//Normalized Tangent vector
};



//--------------------------------------------------------------------------------------
// SkinVertTB skins a single vertex
//--------------------------------------------------------------------------------------
SkinnedInfo SkinVertTB( VSSkinnedIn Input )
{
    SkinnedInfo Output = (SkinnedInfo)0;
    
    float4 Pos = float4(Input.position,1);
    float3 Norm = Input.normal;
    float3 Tan = Input.tangent;
	
    //Test
    uint iBoneTest = Input.bones.x;
    matrix mm = g_mTexBoneWorld[0];
    //float4 Pos = float4(1,1,1,1);
	Output.position = mul( Pos, mm );
	//Output.position.y = Input.position.y;
	//Output.position.z = Input.position.z;
	//Output.position.w = 1;
	//Output.position = g_mTexBoneWorld[3];
    return Output;
}

//--------------------------------------------------------------------------------------
// SkinVertTB skins a single vertex
//--------------------------------------------------------------------------------------
SkinnedInfo SkinVertTextureBuffer( VSSkinnedIn Input )
{
    SkinnedInfo Output = (SkinnedInfo)0;
    
    float4 Pos = float4(Input.position,1);
    float3 Norm = Input.normal;
    float3 Tan = Input.tangent;

    //uint iBone *= 4;
    float4 row1 = g_txTexBoneWorld.Load( float2(0,     0) );
    float4 row2 = g_txTexBoneWorld.Load( float2(0 + 1, 0) );
    float4 row3 = g_txTexBoneWorld.Load( float2(0 + 2, 0) );
    float4 row4 = g_txTexBoneWorld.Load( float2(0 + 3, 0) );
       
    matrix mm = float4x4( row1, row2, row3, row4 );
    //float4 Pos = float4(1,1,1,1);
	Output.position = mul( Pos, mm );
	//Output.position.y = Input.position.y;
	//Output.position.z = Input.position.z;
	//Output.position.w = 1;
	//Output.position = g_mTexBoneWorld[3];
    return Output;
}


//--------------------------------------------------------------------------------------
// SkinVertTB skins a single vertex
//--------------------------------------------------------------------------------------
SkinnedInfo SkinBuffer( VSSkinnedIn Input )
{
    SkinnedInfo Output = (SkinnedInfo)0;
    
    float4 Pos = float4(Input.position,1);
    float3 Norm = Input.normal;
    float3 Tan = Input.tangent;

    uint iBone = 0;
    float4 row1 = g_BufferBoneWorld.Load( iBone );
    float4 row2 = g_BufferBoneWorld.Load( iBone + 1 );
    float4 row3 = g_BufferBoneWorld.Load( iBone + 2 );
    float4 row4 = g_BufferBoneWorld.Load( iBone + 3 );
       
    matrix mm = float4x4( row1, row2, row3, row4 );
    //float4 Pos = float4(1,1,1,1);
	Output.position = mul( Pos, mm );
	//Output.position.y = Input.position.y;
	//Output.position.z = Input.position.z;
	//Output.position.w = 1;
	//Output.position = g_mTexBoneWorld[3];
    return Output;
}



//--------------------------------------------------------------------------------------
// SkinVert skins a single vertex
//--------------------------------------------------------------------------------------
SkinnedInfo SkinVert( VSSkinnedIn Input )
{
    SkinnedInfo Output = (SkinnedInfo)0;
    
    float4 Pos = float4(Input.position,1);
    float3 Norm = Input.normal;
    float3 Tan = Input.tangent;

    //Test
    uint iBoneTest = Input.bones.x;
    matrix mm = g_mConstBoneWorld[0];
    	
	Output.position = mul( Pos, mm );
    return Output;

    //Bone0
    uint iBone = Input.bones.x;
    float fWeight = Input.weights.x;
    matrix m = g_mConstBoneWorld[iBone];
    Output.position += fWeight * mul( Pos, m );
    Output.normal += fWeight * mul( Norm, (float3x3)m );
    Output.tangent += fWeight * mul( Tan, (float3x3)m );
	
    //Bone1
    iBone = Input.bones.y;
    fWeight = Input.weights.y;
    m = g_mConstBoneWorld[iBone];
    Output.position += fWeight * mul( Pos, m );
    Output.normal += fWeight * mul( Norm, (float3x3)m );
    Output.tangent += fWeight * mul( Tan, (float3x3)m );
    
    //Bone2
    iBone = Input.bones.z;
    fWeight = Input.weights.z;
    m = g_mConstBoneWorld[iBone];
    Output.position += fWeight * mul( Pos, m );
    Output.normal += fWeight * mul( Norm, (float3x3)m );
    Output.tangent += fWeight * mul( Tan, (float3x3)m );
    
    //Bone3
    iBone = Input.bones.w;
    fWeight = Input.weights.w;
    m = g_mConstBoneWorld[iBone];
    Output.position += fWeight * mul( Pos, m );
    Output.normal += fWeight * mul( Norm, (float3x3)m );
    Output.tangent += fWeight * mul( Tan, (float3x3)m );
    return Output;
}

//--------------------------------------------------------------------------------------
// Vertex shader used for skinning and streaming out
//--------------------------------------------------------------------------------------
VSStreamOut vsMain(VSSkinnedIn input )
{
    VSStreamOut output = (VSStreamOut)0;

    output.Pos = float4(input.position.xyz,1);
    output.Norm = input.normal;
    output.Tangent = normalize( input.tangent );
    output.Tex = input.tex;
    //TextureBuffer
	SkinnedInfo vSkinned = SkinBuffer(input);
	output.Pos = vSkinned.position;


    //TextureBuffer
	//SkinnedInfo vSkinned = SkinVertTextureBuffer(input);
	//output.Pos = vSkinned.position;
	
	//TBuffer
    //SkinnedInfo vSkinned = SkinVertTB( input );
    //output.Pos = vSkinned.position;

	//CBuffer
    //SkinnedInfo vSkinned = SkinVert( input );
    //output.Pos = vSkinned.position;

    //output.Norm = vSkinned.normal;
    //output.Tangent = normalize( vSkinned.tangent );
    //output.Tex = input.tex;
    
    return output;
}