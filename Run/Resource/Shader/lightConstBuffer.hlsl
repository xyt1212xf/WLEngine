

struct LightData
{
	float4 color;
	float3 position;
	float  fRange;
};

StructuredBuffer<LightData> GlobalLight : register(t3);

cbuffer EnvironmentLight : register(b7)
{
	float4 AmbientLight;
	float4 DiffuseLight;
	float3 LightDirection;		
	matrix LightView;
	matrix LightProjection;
};



