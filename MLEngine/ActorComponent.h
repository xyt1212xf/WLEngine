#pragma once
#include "Component.h"
namespace ML
{
	class CActor;

	class MLENGINE_API CActorComponent : public CComponent
    {
    public:
        CActorComponent();
        virtual ~CActorComponent();

		// UE: UActorComponent::Owner / GetOwner(). 组件访问自己所属 Actor 的
		// 唯一通道（Lua 侧的 self:GetOwner()、组件读 Actor tag 等都走这里）。
		// 裸反向指针，不参与所有权：Actor 的析构负责 delete 组件。
		CActor* GetOwner() const { return Owner; }

    protected:
		// 只有 CActor::AddComponent 能设，外部拿不到写口。
		friend class CActor;
		void SetOwner(CActor* InOwner) { Owner = InOwner; }

    private:
		CActor* Owner = nullptr;
    };
}
