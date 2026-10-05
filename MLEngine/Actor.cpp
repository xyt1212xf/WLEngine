#include "Actor.h"
#include "Component.h"
namespace ML
{

	CActor::~CActor()
	{
		for (int32 i = 0; i < Components.Num(); ++i)
		{
			delete Components[i];
		}
		Components.Empty();
	}

	void CActor::SetActorLocation(const Vec3F& Pos)
	{
		Location = Pos;
	}

}
