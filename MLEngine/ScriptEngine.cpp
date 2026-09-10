#include "ScriptEngine.h"
#include "WLEngine.h"
#include "Console.h"
#include "LuaBind.h"
#include "Vector.h"
namespace WL
{	
	bool initialiseGameplayTags(const char* szFile)
	{
		return GEngine->getGameplayTagsMgr()->loadGameplayTags(szFile);
	}

	void unInitialiseGameplayTags()
	{

	}

	void exitGame()
	{
		GEngine->exitGame();
	}
	// 获取成员函数
	int getVec3(lua_State* L)
	{
		Vec3F* vec = static_cast<Vec3F*>(lua_touserdata(L, 1));
		const char* field = lua_tostring(L, 2);
		if (std::strcmp(field, "x") == 0) 
		{
			lua_pushnumber(L, vec->x);
		}
		else if (std::strcmp(field, "y") == 0) 
		{
			lua_pushnumber(L, vec->y);
		}
		else if (std::strcmp(field, "z") == 0) 
		{
			lua_pushnumber(L, vec->z);
		}
		else 
		{
			lua_pushnil(L);
		}
		return 1;
	}

	// 设置成员函数
	int setVec3(lua_State* L) 
	{
		Vec3F* vec = static_cast<Vec3F*>(lua_touserdata(L, 1));
		const char* field = lua_tostring(L, 2);
		int value = lua_tonumber(L, 3);
		if (field[0] == 'x')
		{
			vec->x = value;
		}
		else if (field[0] == 'y')
		{
			vec->z = value;
		}
		else if (field[0] == 'z')
		{
			vec->z = value;
		}
		return 1;
	}

	// 创建 Vector 对象并将其推入 Lua
	int createVec3(lua_State* L) 
	{
		Vec3F* v = static_cast<Vec3F*>(lua_newuserdata(L, sizeof(Vec3F)));
		luaL_getmetatable(L, "Vec3MetaTable");
		lua_setmetatable(L, -2);
		return 1; // 返回 userdata
	}

	void registerVec3(lua_State* L)
	{
		luaL_newmetatable(L, "Vec3MetaTable");

		lua_pushcfunction(L, getVec3);
		lua_setfield(L, -2, "__index");

		lua_pushcfunction(L, setVec3);
		lua_setfield(L, -2, "__newindex");

		lua_pop(L, 1); // Pop the metatable

		lua_register(L, "Vec3", createVec3);
	}

	CScriptEngine::CScriptEngine()
	{

	}

	CScriptEngine::~CScriptEngine()
	{

	}

	bool CScriptEngine::initialize()
	{
#ifdef _DEBUG
//		ShowConsole(true);
#endif
		CScriptEntity::bOverRegister = false;
		mpLuaState = luaL_newstate();
		luaL_openlibs(mpLuaState);
	
		int n = lua_gettop(mpLuaState);
		// 设置新的路径
		const char* new_path = "./script/?.lua;./script/frame/.?.lua";

		// 将新路径推入栈顶
		lua_getglobal(mpLuaState, "package");
		lua_getfield(mpLuaState, -1, "path");  // 获取package.path
		lua_pushstring(mpLuaState, new_path);  // 将新路径作为字符串压入栈顶
		lua_concat(mpLuaState, 2);             // 连接两个字符串，得到新的路径字符串
		lua_setfield(mpLuaState, -2, "path");  // 设置package.path为新的路径字符串
		lua_settop(mpLuaState, n);
	
		for (auto item : *CScriptEntity::spRegisterClassObject)
		{
			item->registerClass();
			item->registerMember();
		}
		CRegisterFun<void>::registrFun<void(*)(), &exitGame>(GEngine->getLuaState(), "exitGame");
		registerVec3(mpLuaState);

		static bool isRun = true;
		if (isRun)
		{
			const char* content = "_LUAC = {}\n"
				"function GlobalMessageMap(wndId)\n"
				"	_LUAC.messageMap = _LUAC.messageMap or {}\n"
				"	_LUAC.messageMap[wndId] = _LUAC.messageMap[wndId] or {}\n"
				"	return 1\n"
				"end\n";
			doString(content);
			isRun = false;
			const char* deepCopy =  "function deepCopy(obj, seen)\n"
									"	--Handle non - tables and previously - seen tables.\n"
									"	if type(obj) ~= 'table' then\n"
									"		return obj\n"
									"	end\n"
									"	if seen and seen[obj] then\n"
									"		return seen[obj]\n"
									"	end\n"
									"-- New table; mark it as seen and copy recursively.\n"
									"	local s = seen or {}\n"
									"	local res = {}\n"
									"	s[obj] = res\n"
									"	-- Copy metatable\n"
									"	local mt = getmetatable(obj)\n"
									"	if mt then\n"
									"		setmetatable(res, deepCopy(mt, s))\n"
									"	end\n"
									"	-- Copy key - value pairs\n"
									"	for k, v in pairs(obj) do\n"
									"		res[deepCopy(k, s)] = deepCopy(v, s)\n"
									"	end\n"
									"	return res\n"
									"end\n";
			doString(deepCopy);

			CRegisterFun<bool>::registrFun<bool(*)(const char*), &initialiseGameplayTags, const char*>(mpLuaState, "initialiseGameplayTags");
			CRegisterFun<void>::registrFun<void(*)(), &unInitialiseGameplayTags >(mpLuaState, "unInitialiseGameplayTags");
		}
		return true;
	}


	void CScriptEngine::unInitialize()
	{
		for( auto item : mpLuaStateArray )
		{
			lua_close(item.second);
		}
		mpLuaStateArray.clear();
		lua_close(mpLuaState);
		mpLuaState = nullptr;
	}

	bool CScriptEngine::doFile(const std::string& filename, const std::string& entryMain /*= ""*/, int nRetrun /* = 0*/)
	{
		return doFile(GEngine->getLuaState(), filename, entryMain,nRetrun);
	}

	
	bool CScriptEngine::doFile(lua_State* pLuaState, const std::string& filename, const std::string& entryMain /*= ""*/, int nRetrun/* = 0*/)
	{
		lua_State* plua = pLuaState;
		try
		{
			int bRet = luaL_loadfile(plua, filename.c_str());
			if (bRet)
			{
				throw false;
			}
			bRet = lua_pcall(plua, 0, nRetrun, 0);
			if (bRet)
			{
				throw false;
			}
			if (entryMain.length() > 0)
			{
				lua_getglobal(plua, entryMain.c_str());
				bRet = lua_pcall(plua, 0, 0, 0);
				if (bRet)
				{
					throw false;
				}
			}
		}
		catch (bool)
		{
			int t = lua_type(plua, -1);
			const char* err = lua_tostring(plua, -1);
			std::cout<<err;
			lua_pop(plua, 1);
			return false;
		}
		return true;
	}

	bool CScriptEngine::doString(const std::string& content)
	{
		lua_State* plua = GEngine->getLuaState();
		if (!content.empty())
		{
			int a = luaL_dostring(plua, content.c_str());
			return true;
		}
		return false;
	}

	lua_State* CScriptEngine::getLuaState()
	{
		return mpLuaState;
	}

	lua_State* CScriptEngine::getLuaState(const std::string& szLuaStateName)
	{
		lua_State* pLuaState = nullptr;
		auto iter = mpLuaStateArray.find(szLuaStateName);
		if (iter != mpLuaStateArray.end())
		{
			pLuaState = iter->second;
		}
		return pLuaState;
	}

	lua_State* CScriptEngine::createLuaState(const std::string& szLuaStateName)
	{
		lua_State* pLuaState = luaL_newstate();
		auto [iter, insert] = mpLuaStateArray.try_emplace(szLuaStateName, pLuaState);
		if (insert)
		{
			luaL_openlibs(pLuaState);
		}
		else
		{
			lua_close(pLuaState);
		}
		return pLuaState;
	}


	void CScriptEngine::releaseLuaState(const std::string& szLuaStateName)
	{
		auto iter = mpLuaStateArray.find(szLuaStateName);
		if (iter != mpLuaStateArray.end())
		{
			lua_close(iter->second);
			mpLuaStateArray.erase(iter);
		}
	}

	void CScriptEngine::registerCFunction()
	{

	}



	void CScriptEngine::_update(UINT32 dTime)
	{

	}

}
