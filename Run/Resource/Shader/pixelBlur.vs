////////////////////////////////////////////////////////////////////////////////
// Filename: light.vs
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer LastMatrixBuffer : register(b5)
{
	matrix lastFrameWorldMT;
	matrix lastFrameViewMT;
	matrix lastprojectionMT;
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

struct VertexOutputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
	float2 velocityUV : TEXCOORD1;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
VertexOutputType vsShader(VertexInputType input)
{
	// Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;
	
    VertexOutputType output;
    
	float3 vNormalWorldSpace;
    float4 vPosProjSpaceCurrent; 
    float4 vPosProjSpaceLast; 
  

    vNormalWorldSpace = normalize(mul(input.normal, (float3x3)worldMatrix)); // normal (world space)
	
	// Transform from object space to homogeneous projection space
    vPosProjSpaceCurrent = mul(input.position, worldMatrix);
    vPosProjSpaceCurrent = mul(vPosProjSpaceCurrent, viewMatrix);
    vPosProjSpaceCurrent = mul(vPosProjSpaceCurrent, projectionMatrix);
	
	output.position = vPosProjSpaceCurrent;
		
	vPosProjSpaceLast = mul(input.position, lastFrameWorldMT);
    vPosProjSpaceLast = mul(vPosProjSpaceLast, lastFrameViewMT);
    vPosProjSpaceLast = mul(vPosProjSpaceLast, lastprojectionMT);
	
    vPosProjSpaceCurrent.xy /= vPosProjSpaceCurrent.w;
    vPosProjSpaceLast.xy /= vPosProjSpaceLast.w;
	
    // Vertex's velocity (in non-homogeneous projection space) is the position this frame minus 
    // its position last frame.  This information is stored in a texture coord.  The pixel shader 
    // will read the texture coordinate with a sampler and use it to output each pixel's velocity.
    float2 velocity = vPosProjSpaceCurrent - vPosProjSpaceLast;    
    
    // The velocity is now between (-2,2) so divide by 2 to get it to (-1,1)
    velocity /= 2.0f;   

    // Store the velocity in a texture coord
    output.velocityUV = velocity;

    output.tex = input.tex;


    return output;
}