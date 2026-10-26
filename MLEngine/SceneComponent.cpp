#include "SceneComponent.h"
#include "Scene.h"
#include "World.h"
#include "MLEngine.h"
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
		MarkRenderTransformDirty();
	}

	void CSceneComponent::SetRotation(const CQuaternion& InRotation)
	{
		Rotation = InRotation;
		MarkRenderTransformDirty();
	}

	void CSceneComponent::SetScale(const Vec3F& InScale)
	{
		Scale = InScale;
		MarkRenderTransformDirty();
	}

	void CSceneComponent::PushSceneCommand(std::function<void(FScene&)> InCommand)
	{
		// 还没注册进场景（id 为 0），或引擎/世界还不存在 —— 直接丢弃。
		// 这覆盖了构造期和关卡加载期调用 SetLocation 的情况。
		if (0 == RenderProxyId || nullptr == GEngine)
		{
			return;
		}
		if (CWorld* World = GEngine->GetWorld())
		{
			World->EnqueueRenderCommand(std::move(InCommand));
		}
	}

	void CSceneComponent::MarkRenderTransformDirty()
	{
		if (0 == RenderProxyId)
		{
			return;
		}
		// 关键：世界矩阵在**游戏线程**算好后按值捕获投递。
		// 渲染线程收到的是一份快照，不存在"读到写了一半的矩阵"的问题。
		const uint32   Id    = RenderProxyId;
		const FMatrix44 World = GetWorldMatrix();
		PushSceneCommand([Id, World](FScene& Scene)
			{
				Scene.SetTransform(Id, World);
			});
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
