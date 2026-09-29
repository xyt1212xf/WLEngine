#pragma once
#include "Component.h"
#include "Vector.h"

namespace ML
{
	class CTransformComponent : public CComponent
	{
	public:
		CTransformComponent();
		virtual ~CTransformComponent();
	public:
		Vec3F Location;
//		FQuat Rotation = FQuat::Identity;
		Vec3F Scale;
	};
}