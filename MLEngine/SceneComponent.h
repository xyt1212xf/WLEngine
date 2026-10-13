#pragma once
#include "ActorComponent.h"
#include "Vector.h"
#include "Quaternion.h"
#include "Matrix.h"

namespace ML
{
	class MLENGINE_API CSceneComponent : public CActorComponent
    {
    public:
        CSceneComponent();
        virtual ~CSceneComponent();
		void SetLocation(const Vec3F& InLocation);
		void SetRotation(const CQuaternion& InRotation);
		void SetScale(const Vec3F& InScale);
		const Vec3F& GetLocation() const { return Location; }
		const CQuaternion& GetRotation() const { return Rotation; }
		const Vec3F& GetScale() const { return Scale; }
		FMatrix44 GetWorldMatrix() const;

	protected:
		Vec3F       Location;
		CQuaternion Rotation = CQuaternion(0.0f, 0.0f, 0.0f, 1.0f);
		Vec3F       Scale = Vec3F(1, 1, 1);

    };
}
