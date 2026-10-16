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
		// 本地 TRS（行向量约定：p * M，故顺序 Scale→Rotation→Translation）
		FMatrix44 TranslationMatrix = FMatrix44::MakeTranslation(Location);
		FMatrix44 RotationMatrix = FMatrix44::MakeRotation(Rotation);
		FMatrix44 ScaleMatrix = FMatrix44::MakeScale(Scale);
		FMatrix44 LocalMatrix = ScaleMatrix * RotationMatrix * TranslationMatrix;

		// UE: 世界变换 = 相对变换 ∘ 父级世界变换。没有父级时（root）就是它自己。
		// 这样 AActor::SetActorTransform（只写 root）才能传导到挂在 root 下的组件。
		return (nullptr != AttachParent)
			? (LocalMatrix * AttachParent->GetWorldMatrix())
			: LocalMatrix;
	}
}
