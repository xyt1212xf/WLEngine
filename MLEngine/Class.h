#pragma once
#include "Common.h"

namespace WL
{
	struct StructInfo
	{
		std::string Name;                      // 类名，方便打印
		StructInfo* SuperStruct;                // 父类指针（单继承）
		std::vector<StructInfo*> InheritanceCache; // 运行时缓存：存放所有
	};

	class CClass : public StructInfo
	{
	public:
		bool IsChildOfWalk(const StructInfo* Other) const;
		bool IsChildOfArray(const StructInfo* Other) const;

	
	protected:	
		void BuildInheritanceCache();
	};
}