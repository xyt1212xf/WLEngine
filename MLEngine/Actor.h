#pragma once
#include "Object.h"
#include "Array.h"
namespace ML
{
	class CComponent;

	class CActor : public CObject
	{
	public:
		CActor() = default;
		virtual ~CActor();

		void SetActorLocation(const Vec3F& Pos);
		const Vec3F& GetActorLocation() const { return Location; }

		template<class T>
		T* AddComponent()
		{
			T* Comp = new T();
			Components.Add(Comp);
			return Comp;
		}

		const TArray<CComponent*>& GetComponents() const { return Components; }

	private:
		Vec3F Location;
		TArray<CComponent*> Components;
	};
}
