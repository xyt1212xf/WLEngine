#pragma once
#include "Refcount.h"
#include "ScriptEntity.h"
namespace WL
{
	struct GameplayAttribute
	{
		std::string getName() const
		{
			return mAttributeName;
		}
		
		bool operator==(const GameplayAttribute& Other) const
		{
			return true;
		}
		
		bool operator!=(const GameplayAttribute& Other) const
		{
			return true;
		}

		std::string mAttributeName = "";
	};

	class WLENGINE_API CAttributeSet : public CScriptEntity
	{
	public:
		CAttributeSet();
		virtual ~CAttributeSet();

		void doBreak()
		{
			int a = 0;
		}
		virtual void preAttributeChange(const GameplayAttribute& Attribute, float& NewValue) {}

		/** Called just after any modification happens to an attribute. */
		virtual void postAttributeChange(const GameplayAttribute& Attribute, float OldValue, float NewValue) {}

	private:
		DeclareScriptClass(CAttributeSet);
	};
}