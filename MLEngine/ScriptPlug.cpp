#include "ScriptPlug.h"
namespace ML
{
	CScriptPlug::CScriptPlug()
	{

	}

	CScriptPlug::~CScriptPlug()
	{

	}

	bool CScriptPlug::Initialise()
	{
		mpLuaState = luaL_newstate();
		luaL_openlibs(mpLuaState);
		// 设置新的路径
		const char* new_path = "./script/?.lua;./script/frame/.?.lua";

		// 将新路径推入栈顶
		lua_getglobal(mpLuaState, "package");
		lua_getfield(mpLuaState, -1, "path");  // 获取package.path
		lua_pushstring(mpLuaState, new_path);  // 将新路径作为字符串压入栈顶
		lua_concat(mpLuaState, 2);             // 连接两个字符串，得到新的路径字符串
		lua_setfield(mpLuaState, -2, "path");  // 设置package.path为新的路径字符串
		return true;
	}

	bool CScriptPlug::UnInitialise()
	{
		if (mpLuaState)
		{
			lua_close(mpLuaState);
			mpLuaState = nullptr;
		}
		return true;
	}

	void CScriptPlug::Process(float DeltaSeconds) const
	{

	}

	void CScriptPlug::Start()
	{

	}

}
