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
		virtual void Tick(float DeltaSeconds) {};
		void SetActorLocation(const Vec3F& Pos);
		const Vec3F& GetActorLocation() const { return Location; }
		void BeginPlay();

		template<class T>
		T* AddComponent()
		{
			T* Comp = new T();
			Components.Add(Comp);
			return Comp;
		}

		const TArray<CComponent*>& GetComponents() const { return Components; }
		
		void SetActorTickEnabled(bool b) { bCanTick = b; }
		bool CanTick() const { return bCanTick; }

	private:
		bool bCanTick = true;

	private:
		Vec3F Location;
		TArray<CComponent*> Components;
	};
}
