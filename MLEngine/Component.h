#pragma once
#include "NonCopyable.h"

namespace ML
{
	class CComponent : public CNonCopyable
	{
	public:
		CComponent() = default;
		virtual ~CComponent();
		const std::string& getComponentName() const;
		void setComponentName(const std::string& szName);
		virtual void tick([[maybe_unused]] float DeltaSeconds){};
		
	protected:
		std::string mComponentName = "Component";
	};


}