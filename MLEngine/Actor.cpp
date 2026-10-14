#include "Actor.h"
#include "SceneComponent.h"
namespace ML
{

	CActor::CActor()
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
		RootComponent->SetLocation(Pos);
	}


	const Vec3F& CActor::GetActorLocation() const
	{
		return RootComponent->GetLocation();
	}

	void CActor::BeginPlay()
	{

	}

}
