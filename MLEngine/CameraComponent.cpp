#include "CameraComponent.h"
namespace ML
{

	void CCameraComponent::GetCameraView(FMinimalViewInfo& OutView) const
	{
		const FMatrix44& M = GetWorldMatrix();
		// 与 CLightSceneProxy 同一套组件矩阵约定：row2 = 朝向，row3 = 位置
		//（CSceneComponent::GetWorldMatrix，行向量 LH，Forward = +Z）。
		const Vec3F Forward(M.m[2][0], M.m[2][1], M.m[2][2]);
		OutView.Location = Vec3F(M.m[3][0], M.m[3][1], M.m[3][2]);
		OutView.Rotation = Forward;
		OutView.FOV = FieldOfView;
		OutView.NearZ = NearZ;
		OutView.FarZ = FarZ;
		OutView.AspectRatio = AspectRatio;
	}

	void CCameraComponent::RegisterComponentWithWorld(CWorld* InWorld)
	{

	}

}
