#pragma once
#include "Common.h"
#include "Object.h"
#include "SceneComponent.h"
namespace ML
{
	class CSceneComponent;
	class CComponent;
	class MLENGINE_API CActor : public CObject
	{
	public:
		CActor();
		virtual ~CActor();
		virtual void Tick(float DeltaSeconds) {};
		void SetActorLocation(const Vec3F& Pos);
		const Vec3F& GetActorLocation() const;
		void BeginPlay();

		template<class T>
		T* AddComponent()
		{
			T* Comp = new T();
			Components.push_back(Comp);
			// 编译期提问：T 是不是 CSceneComponent 的子类
			if constexpr (std::is_convertible_v<T*, CSceneComponent*>)
			{
				// 只有子类才会生成这段代码；且 Comp 此时隐式转成 CSceneComponent*
				if (nullptr == RootComponent) 
				{
					RootComponent = Comp; 
				}	
			}
			return Comp;
		}

		const std::vector<CComponent*>& GetComponents() const { return Components; }
		
		void SetActorTickEnabled(bool b) { bCanTick = b; }
		bool CanTick() const { return bCanTick; }

	private:
		bool bCanTick = true;

	private:
		CSceneComponent* RootComponent = nullptr;
		std::vector<CComponent*> Components;
	};
}
