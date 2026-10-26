#pragma once
#include "NonCopyable.h"

namespace ML
{
	class CWorld;

	class MLENGINE_API CComponent : public CNonCopyable
	{
	public:
		CComponent() = default;
		virtual ~CComponent();
		const std::string& getComponentName() const;
		void setComponentName(const std::string& szName);
		virtual void tick([[maybe_unused]] float DeltaSeconds){};

		// UE: UActorComponent::RegisterComponentWithWorld(UWorld*)。
		// 注册入口放在组件列表的元素类型上（UE 里那个类型是 UActorComponent，
		// 本项目是 CComponent），这样 CWorld::InitActorsForPlay 只做纯多态分发，
		// 不认识任何具体组件类型 —— 以后新增可绘制组件只需在
		// CPrimitiveComponent 一侧覆写，CWorld 不用改。
		// 基类没有渲染状态，默认无事可做。
		virtual void RegisterComponentWithWorld(CWorld* InWorld);
		
	protected:
		std::string mComponentName = "Component";
	};


}
