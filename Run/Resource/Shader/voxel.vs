////////////////////////////////////////////////////////////////////////////////
// Filename: light.vs
////////////////////////////////////////////////////////////////////////////////
#include "globalConstBuffer.hlsl"

//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
    float4 position : POSITION;
   	float3 normal : NORMAL;
    float2 tex : TEXCOORD;
	float3 alpha : COLOR;
	float3 weight : BLENDWEIGHT;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
	float4 worldPos : POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 alpha : TEXCOORD1;
    float4 lightViewPosition : TEXCOORD2;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType vsMain(VertexInputType input)
{
    PixelInputType output;
    

	input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.worldPos = output.position;
	output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
	//output.position.w = 10;
	
	// Store the texture coordinates for the pixel shader.
	output.tex = input.tex;
    
	// Calculate the normal vector against the world matrix only.
	output.normal = mul(input.normal, (float3x3)worldMatrix);
	
	output.alpha = input.alpha;
	
	output.lightViewPosition = mul(input.position, worldMatrix);
    output.lightViewPosition = mul(output.lightViewPosition, lightViewMatrix);
    output.lightViewPosition = mul(output.lightViewPosition, lightProjectionMatrix);

    return output;
}