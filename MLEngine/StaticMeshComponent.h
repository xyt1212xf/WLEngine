#pragma once
#include "Component.h"
#include "Matrix.h"
#include "Quaternion.h"
#include "Vector.h"
namespace ML
{
	class CStaticMesh;
	struct FMeshBatch;
	class CStaticMeshComponent : public CComponent
	{
	public:
		CStaticMeshComponent();
		virtual ~CStaticMeshComponent();
		void GetMeshBatch(FMeshBatch& OutBatch,const FMatrix44& InViewProj) const;
		void SetLocation(const Vec3F& InLocation);
		void SetRotation(const CQuaternion& InRotation);
		void SetScale(const Vec3F& InScale);
		Vec3F GetLocation() const { return Location; }
		CQuaternion GetRotation() const { return Rotation; }
		Vec3F GetScale() const { return Scale; }

		FMatrix44 GetWorldMatrix() const;

	protected:
		CStaticMesh* StaticMesh = nullptr;
		bool bVisible = false;

		// 变换（位置/旋转/缩放）
		Vec3F Location;
		CQuaternion  Rotation;
		Vec3F Scale = Vec3F(1,1,1);
	};
}