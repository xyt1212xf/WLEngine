#pragma once
#include "Common.h"
#include "Refcount.h"
namespace WL
{
	struct ObjectHeader 
	{
		ObjectHandle	SelfHandle = INVALID_HANDLE;
		UINT32			ReferenceTableIndex = INVALID_HANDLE; // 指向全局引用表的索引
		UINT32			ObjectSize = 0;
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
	inline INT32 ObjectHeadSize = sizeof(ObjectHeader);
}