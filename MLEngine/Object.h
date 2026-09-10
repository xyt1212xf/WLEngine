#pragma once
#include "Common.h"
#include "Refcount.h"
namespace WL
{
	struct FObjectHeader 
	{
		ObjectHandle	SelfHandle = INVALID_HANDLE;
		UINT32			ObjectSize = 0;
		char			Name[32];
		bool			bMarked = false;
	};

	class  WLENGINE_API CObject : public CRefcount
	{
		friend class CGCObjectMgr;
	public:
		CObject() = default;
		CObject(const std::string& InName);
		CObject(std::string&& InName) noexcept;
		const std::string& GetName() const;
		
	private:
		std::string Name;
 
	};
	inline INT32 ObjectHeadSize = sizeof(FObjectHeader);
}