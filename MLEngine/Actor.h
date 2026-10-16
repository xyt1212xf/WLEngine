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
		CActor(const std::string& InName);
		virtual ~CActor();
		virtual void Tick(float DeltaSeconds) {};
		void SetActorLocation(const Vec3F& Pos);
		const Vec3F& GetActorLocation() const;
		void BeginPlay();

		// ---- UE: AActor ----
		// UE: AActor::GetRootComponent(). The root is created by the ctor as a
		// plain CSceneComponent, exactly like AActor's default RootComponent.
		CSceneComponent* GetRootComponent() const { return RootComponent; }

		// UE: AActor::SetActorTransform(const FTransform&).
		// The level file stores Rotation as Euler degrees (editor-style, like
		// FRotator); memory keeps a quaternion, like FTransform::Rotation.
		void SetActorTransform(const Vec3F& InLocation, const CQuaternion& InRotation, const Vec3F& InScale);

		// UE: AActor::Tags (TArray<FName>)
		void AddTag(const std::string& InTag);
		const std::vector<std::string>& GetTags() const { return Tags; }

		// UE: AActor::GetActorLabel() - editor display name, distinct from the
		// internal object name (UE keeps them separate too).
		void SetActorLabel(const std::string& InLabel);
		const std::string& GetActorLabel() const { return Label; }

		// Lua class path. Stands in for the UClass* UE resolves through
		// AActor::GetClass(); the behaviour itself lives in Run/Script/actor/*.lua.
		void SetLuaClass(const std::string& InClass);
		const std::string& GetLuaClass() const { return LuaClass; }

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
				else
				{
					// UE: 非 root 的 scene component 挂到 root 下，否则
					// AActor::SetActorTransform（只写 root）传不到它身上，
					// 组件会永远停在原点。
					Comp->AttachToComponent(RootComponent);
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
		std::vector<std::string> Tags;
		std::string Label = "";
		std::string LuaClass = "";
	};
}
