

struct VSStreamIn
{
    float4 position	: POSITION;			//Position
    float3 normal : NORMAL;			//Normal
    float2 tex	: TEXCOORD;		    //Texture coordinate
    float3 tangent : TANGENT;		//Normalized Tangent vector
};


struct GSOutput
{
    float4 position : POSITION;
    float3 normal : NORMAL;
	float2 tex : TEXCOORD;
	float3 tangent : TANGENT;
};

[maxvertexcount(1)]
void gsMain(point VSStreamIn input[1], inout PointStream<GSOutput> OutputStream )
{
	GSOutput output;
	
	output.position = input[0].position;
	output.normal = input[0].normal;
	output.tex = input[0].tex;
	output.tangent = input[0].tangent;
	
	OutputStream.Append(output);
}

