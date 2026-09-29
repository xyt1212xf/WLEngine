#pragma once
#include "Object.h"
namespace ML
{
	class CActor : public CObject
	{
	public:
		CActor() = default;
		virtual ~CActor();
		void SetActorLocation( const Vec3F& Pos);
	};
}
