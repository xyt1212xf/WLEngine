////////////////////////////////////////////////////////////////////////////////
// Filename: deferred.vs
////////////////////////////////////////////////////////////////////////////////

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


//////////////
// TYPEDEFS //
//////////////
struct PSSkinnedIn
{
    float4 position	: SV_Position;		//Position
    float3 vPosition : POSWORLD;			//world space Pos
    float3 normal : NORMAL;			//Normal
    float2 tex	: TEXCOORD;		    //Texture coordinate
    float3 tangent : TANGENT;		//Normalized Tangent vector
};

struct VSStreamOut
{
    float4 position	: POSITION;			//Position
    float3 normal : NORMAL;			//Normal
    float2 tex	: TEXCOORD;		    //Texture coordinate
    float3 tangent : TANGENT;		//Normalized Tangent vector
};

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PSSkinnedIn vsMain(VSStreamOut input)
{	
    PSSkinnedIn output;
    output.vPosition = mul( input.position, worldMatrix );
	
    output.position = mul( input.position, worldMatrix );
    output.position = mul( output.position, viewMatrix );
    output.position = mul( output.position, projectionMatrix );

    output.normal = normalize( mul( input.normal, (float3x3)worldMatrix ) );
    output.tangent = normalize( mul( input.tangent, (float3x3)worldMatrix ) );
    output.tex = input.tex;
    
    return output;
}