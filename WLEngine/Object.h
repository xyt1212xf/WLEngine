#pragma once
#include "Common.h"
#include "Refcount.h"
namespace WL
{
	struct ObjectHeader 
	{
		ObjectHandle	SelfHandle;
		UINT32			ReferenceTableIndex; // 指向全局引用表的索引
		char			Name[32];
		bool			bMarked = false;
	};

	class  CObject : public CRefcount
	{
		friend class CGCObjectMgr;
	public:
		CObject() = default;
		CObject(const std::string& InName);
		CObject(std::string&& InName) noexcept;

		void AddReference(CObject* Obj);
		
	private:
		std::string Name;
		bool bMarked = false;     
		std::vector<CObject*> References; 
	};
}