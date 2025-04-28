
#include "globalConstBuffer.hlsl"

//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
    float4 position : POSITION;
  	float3 normal : NORMAL;
    float2 tex : TEXCOORD0;
    float color0 : COLOR0;
    float color1 : COLOR1;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
	float4 worldPos : POSITION;
	float3 normal : NORMAL;
    float2 tex : TEXCOORD0;
    float4 lightViewPosition : TEXCOORD1;
	float color0 : COLOR0;
    float color1 : COLOR1;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType vsShader(VertexInputType input)
{
    PixelInputType output;
	input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.worldPos = mul(input.position, worldMatrix);
	output.position = output.worldPos;
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
	
	
	// Store the texture coordinates for the pixel shader.
	output.tex = input.tex;
	output.color0 = input.color0;
	output.color1 = input.color1;
    
	// Calculate the normal vector against the world matrix only.
	output.normal = mul(input.normal, (float3x3)worldMatrix);
	
	// Normalize the normal vector.
	output.normal = normalize(output.normal);
	
	output.lightViewPosition = mul(input.position, worldMatrix);
    output.lightViewPosition = mul(output.lightViewPosition, lightViewMatrix);
    output.lightViewPosition = mul(output.lightViewPosition, lightProjectionMatrix);
	
    return output;
}