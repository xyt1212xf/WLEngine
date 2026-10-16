#pragma once
#include "Plug.h"
#include "lua.hpp"
namespace ML
{
	class MLENGINE_API CScriptPlug : public CPlug
	{
	public:
		CScriptPlug();
		virtual ~CScriptPlug();
		virtual bool Initialise() override final;
		virtual bool UnInitialise()override final;
		virtual void Process(float DeltaSeconds) const override final;
		virtual void Start() override final;

	private:
		lua_State* mpLuaState = nullptr;
		std::map<std::string, lua_State*> mpLuaStateArray;
	};
}
