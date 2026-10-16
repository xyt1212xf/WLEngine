#include "Actor.h"
#include "SceneComponent.h"
namespace ML
{

	CActor::CActor()
	{
		AddComponent<CSceneComponent>();
	}

	CActor::CActor(const std::string& InName)
		: CObject(InName)
	{
		AddComponent<CSceneComponent>();
	}

	CActor::~CActor()
	{
		for (int32 i = 0; i < Components.size(); ++i)
		{
			delete Components[i];
		}
		Components.clear();
	}

	void CActor::SetActorLocation(const Vec3F& Pos)
	{
		// 三个 Transform 接口统一判空：RootComponent 由构造函数保证非空，
		// 但定位类接口不该在退化情况下直接崩。
		if (nullptr == RootComponent)
		{
			return;
		}
		RootComponent->SetLocation(Pos);
	}


	const Vec3F& CActor::GetActorLocation() const
	{
		if (nullptr == RootComponent)
		{
			static const Vec3F Zero(0.0f, 0.0f, 0.0f);
			return Zero;
		}
		return RootComponent->GetLocation();
	}

	void CActor::SetActorTransform(const Vec3F& InLocation, const CQuaternion& InRotation, const Vec3F& InScale)
	{
		if (nullptr == RootComponent)
		{
			return;
		}
		RootComponent->SetLocation(InLocation);
		RootComponent->SetRotation(InRotation);
		RootComponent->SetScale(InScale);
	}

	void CActor::AddTag(const std::string& InTag)
	{
		Tags.push_back(InTag);
	}

	void CActor::SetActorLabel(const std::string& InLabel)
	{
		Label = InLabel;
	}

	void CActor::SetLuaClass(const std::string& InClass)
	{
		LuaClass = InClass;
	}

	void CActor::BeginPlay()
	{

	}

}
