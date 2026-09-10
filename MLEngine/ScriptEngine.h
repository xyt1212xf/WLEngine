#pragma once
#include "ScriptEntity.h"
#include "EngineBase.h"

namespace WL
{
	class CScriptEngine : virtual public CEngineBase
	{
	public:
		CScriptEngine();
		virtual ~CScriptEngine();

		static bool doFile(lua_State* pLuaState, const std::string& filename, const std::string& entryMain = "", int nRetrun = 0);
		static bool doFile(const std::string& filename, const std::string& entryMain = "", int nRetrun = 0 );
		static bool	doString(const std::string& content);

		template<typename T>
		static T getTableItem(lua_State* pLua, const char* tableName, const char* itemName)
		{
			T value = T();
			int t = lua_gettop(pLua);
			lua_getglobal(pLua, tableName);
			lua_pushstring(pLua, itemName);
			lua_gettable(pLua, -2);
			if (!lua_isnil(pLua, -1))
			{
				value = (T)lua_tonumber(pLua, -1);
			}
			lua_settop(pLua, t);
			return value;
		}

		template<>
		static const char* getTableItem(lua_State* pLua, const char* tableName, const char* itemName)
		{
			int t = lua_gettop(pLua);
			lua_getglobal(pLua, tableName);
			lua_pushstring(pLua, itemName);
			lua_gettable(pLua, -2);
			if (!lua_isnil(pLua, -1))
			{
				return lua_tostring(pLua, -1);
			}
			else
			{
				lua_settop(pLua, t);
				return "";
			}
		}

		bool initialize();
		void unInitialize();
		lua_State* getLuaState();
		lua_State* getLuaState(const std::string& szLuaStateName);
		lua_State* createLuaState(const std::string& szLuaStateName);
		void releaseLuaState(const std::string& szLuaStateName);

	private:
		void registerCFunction();

	protected:
		virtual void _update(UINT32 dTime);

	private:
		lua_State* mpLuaState = nullptr;
		std::map<std::string, lua_State*> mpLuaStateArray;
	};

#define REGIST_FUNCTION(R, F) \
	CRegisterFun<R>::registrFun<R(*)(), &F>(#F);

#define REGIST_FUNCTION_ARGS(R, F, ...) \
	CRegisterFun<R>::registrFun<R(*)(__VA_ARGS__), &F, __VA_ARGS__>(#F);
}

