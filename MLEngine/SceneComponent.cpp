#include "SceneComponent.h"
namespace ML
{
	CSceneComponent::CSceneComponent()
    {
    }
 
    CSceneComponent::~CSceneComponent()
    {
    }

	void CSceneComponent::SetLocation(const Vec3F& InLocation)
	{
		Location = InLocation;
	}

	void CSceneComponent::SetRotation(const CQuaternion& InRotation)
	{
		Rotation = InRotation;
	}

	void CSceneComponent::SetScale(const Vec3F& InScale)
	{
		Scale = InScale;
	}

	FMatrix44 CSceneComponent::GetWorldMatrix() const
	{
		// 构造 TRS 矩阵
		FMatrix44 TranslationMatrix = FMatrix44::MakeTranslation(Location);
		FMatrix44 RotationMatrix = FMatrix44::MakeRotation(Rotation);
		FMatrix44 ScaleMatrix = FMatrix44::MakeScale(Scale);

		// 世界矩阵 = Scale * Rotation * Translation（注意顺序）
		return ScaleMatrix * RotationMatrix * TranslationMatrix;
	}
}
