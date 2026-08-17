#pragma once
#include "Common.h"
#include "Refcount.h"
namespace WL
{
	class  CObject : public CRefcount
	{
		friend class CGCObjectMgr;
	public:
		CObject(){};
		CObject(const std::string& InName);
		CObject(std::string&& InName) noexcept;

		void AddReference(CObject* Obj);
		
	private:
		std::string Name;
		bool bMarked = false;     
		std::vector<CObject*> References; 
	};
}