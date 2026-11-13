#include "LightSceneProxy.h"
namespace ML
{
	CLightSceneProxy::CLightSceneProxy(const FInitParams& InParams)
	{
		Id = InParams.Id;
		Type = InParams.Type;
		ColorIntensity = InParams.ColorIntensity;
		Radius = InParams.Radius;
		FalloffExponent = InParams.FalloffExponent;
		bVisible = InParams.bVisible;
		UpdateTransformDerived(InParams.WorldMatrix);
	}

	void CLightSceneProxy::UpdateTransformDerived(const FMatrix44& InWorldMatrix)
	{
		WorldMatrix = InWorldMatrix;
		Position = Vec3F(InWorldMatrix.m[3][0], InWorldMatrix.m[3][1], InWorldMatrix.m[3][2]);
		Direction = Vec3F(InWorldMatrix.m[2][0], InWorldMatrix.m[2][1], InWorldMatrix.m[2][2]);
		if (Direction.lengthSqrt() > 1e-8)
		{
			Direction.normalize();
		}
		else
		{
			Direction = Vec3F(0.0f, 0.0f, 1.0f);
		}
	}

	void CLightSceneProxy::PackGPULight(FSceneLightGPU& OutLight) const
	{
		OutLight.ColorIntensity = Vec4F(ColorIntensity.x, ColorIntensity.y, ColorIntensity.z, (float)Type);
		OutLight.Direction = Vec4F(Direction.x, Direction.y, Direction.z, 0.0f); // 照射方向
		OutLight.PositionRadius = Vec4F(Position.x, Position.y, Position.z, Radius);
		OutLight.SpotAngles = Vec4F(0.0f, 0.0f, 0.0f, 0.0f);                      // P0 无聚光
	}

}
