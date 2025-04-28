////////////////////////////////////////////////////////////////////////////////
// Filename: softshadow.vs
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer :register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};



//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;	
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;	
	float3 normal : NORMAL;	
    float4 WViewPosition : TEXCOORD1;
    float4 offTex : TEXCOORD2;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType vsMain(VertexInputType input)
{
    PixelInputType output;
	float4 worldPosition;
     
	// Calculate the normal vector against the world matrix only.
    output.normal = mul(input.normal, (float3x3)worldMatrix);
	
    // Normalize the normal vector.
    output.normal = normalize(output.normal);
    
	// Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;
	float3 scale = output.normal;	
	float4 pos = float4(input.position.xyz + scale.xyz, 1) ;
	output.offTex = pos;
	
	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    output.WViewPosition = output.position;
	
	// Store the texture coordinates for the pixel shader.
    //output.tex = input.tex;
 

	
	//float4 offset = float4(output.normal.xyz, 1);
	
	
    //offset = mul(offset, viewMatrix);
    //offset = mul(offset, projectionMatrix);
	
	return output;
}