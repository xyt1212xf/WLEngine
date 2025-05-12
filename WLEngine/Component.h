#pragma once
#include "Common.h"
#include "MemoryMgr.h"

namespace WL
{
	class WLENGINE_API CComponent 
	{
	public:
		CComponent() = default;
		virtual ~CComponent();
		const std::string& getComponentName() const;
		void setComponentName(const std::string& szName);
		virtual void tick([[maybe_unused]] UINT32 dt){};
		
	protected:
		std::string mComponentName = "Component";
	};


}