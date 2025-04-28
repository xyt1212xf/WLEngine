#include "lightConstBuffer.hlsl"

struct CameraVSType
{
    float4 position : SV_POSITION;
};

CameraVSType vsMain()
{
	CameraVSType output;
	output.position = float4(0,0,0,0);
	return output;
}