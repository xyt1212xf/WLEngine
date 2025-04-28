
cbuffer cb0
{
    float4x4 g_mWorldViewProj;
    float4x4 g_mWorld;
};

struct VSStreamOut
{
    float4 Pos	: POSITION;			//Position
    float3 Norm : NORMAL;			//Normal
    float2 Tex	: TEXCOORD;		    //Texture coordinate
    float3 Tangent : TANGENT;		//Normalized Tangent vector
};

struct PSSkinnedIn
{
    float4 Pos	: SV_Position;		//Position
    float3 vPos : POSWORLD;			//world space Pos
    float3 Norm : NORMAL;			//Normal
    float2 Tex	: TEXCOORD;		    //Texture coordinate
    float3 Tangent : TANGENT;		//Normalized Tangent vector
};


//--------------------------------------------------------------------------------------
// Vertex shader used for rendering an already skinned mesh
//--------------------------------------------------------------------------------------
PSSkinnedIn vsMain(VSStreamOut input )
{
    PSSkinnedIn output;
    
    output.Pos = mul( input.Pos, g_mWorldViewProj );
    output.vPos = input.Pos;
    output.Norm = normalize( mul( input.Norm, (float3x3)g_mWorld ) );
    output.Tangent = normalize( mul( input.Tangent, (float3x3)g_mWorld ) );
    output.Tex = input.Tex;
    
    return output;
}