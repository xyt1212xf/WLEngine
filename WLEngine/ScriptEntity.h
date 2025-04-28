#pragma once
#include "Common.h"
#include "lua.hpp"
#include "Entity.h"

#define DeclareScriptClass( Class ) \
public:\
	static const char ClassName[]; \
	static const char MetaName[]; \
	static Class sClass; \
	virtual	void registerClass(); \
	virtual void registerMember();	

#define DefineScriptClass( Class ) \
	Class Class::sClass;\
	const char Class::ClassName[] = #Class; \
	const char Class::MetaName[] = #Class##"_Meta"; 


namespace WL
{
	class WLENGINE_API CScriptEntity : public CEntity
	{
		friend class CScriptEngine;
	public:
		CScriptEntity();
		virtual ~CScriptEntity();
		virtual void registerClass();
		virtual void registerMember();
	
		UINT32		 getRegisterHandle();
		void		 registerHandle();
		void enterScene();
		void leaveScene();

	public:
		static bool bOverRegister;
	private:
		static std::list<CScriptEntity*>* spRegisterClassObject;
		UINT32	mHandle = 0;
	};
}