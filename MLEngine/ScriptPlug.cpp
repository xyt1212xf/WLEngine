#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#endif

#include <string>
#include "ScriptPlug.h"

namespace
{
	// ---- 最小 socket.core 实现（Winsock），供嵌入的 Lua 给 LuaPanda 提供 TCP 套接字 ----
	static const char* SOCKET_CORE_TCP = "socket.core.tcp";

	struct LuaTcpSocket
	{
		SOCKET sock = INVALID_SOCKET;
	};

	static LuaTcpSocket* CheckTcpSocket(lua_State* L)
	{
		return static_cast<LuaTcpSocket*>(luaL_checkudata(L, 1, SOCKET_CORE_TCP));
	}

	static int l_tcp(lua_State* L)
	{
		static bool wsaStarted = false;
		if (!wsaStarted)
		{
			WSADATA wsa;
			WSAStartup(MAKEWORD(2, 2), &wsa);
			wsaStarted = true;
		}
		LuaTcpSocket* s = static_cast<LuaTcpSocket*>(lua_newuserdata(L, sizeof(LuaTcpSocket)));
		s->sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (s->sock == INVALID_SOCKET)
		{
			lua_pushnil(L);
			lua_pushstring(L, "socket create failed");
			return 2;
		}
		luaL_getmetatable(L, SOCKET_CORE_TCP);
		lua_setmetatable(L, -2);
		return 1;
	}

	static int l_settimeout(lua_State* L)
	{
		LuaTcpSocket* s = CheckTcpSocket(L);
		double sec = luaL_optnumber(L, 2, 0.0);
		int ms = static_cast<int>(sec * 1000.0);
		if (s->sock != INVALID_SOCKET)
		{
			setsockopt(s->sock, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char*>(&ms), sizeof(ms));
			setsockopt(s->sock, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<const char*>(&ms), sizeof(ms));
		}
		lua_pushinteger(L, 1);
		return 1;
	}

	static int l_connect(lua_State* L)
	{
		LuaTcpSocket* s = CheckTcpSocket(L);
		const char* host = luaL_checkstring(L, 2);
		int port = static_cast<int>(luaL_checknumber(L, 3));

		addrinfo hints;
		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_INET;
		char portStr[16];
		sprintf_s(portStr, "%d", port);

		addrinfo* res = nullptr;
		if (getaddrinfo(host, portStr, &hints, &res) != 0 || res == nullptr)
		{
			lua_pushnil(L);
			lua_pushstring(L, "host not found");
			return 2;
		}
		sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_port = htons(static_cast<u_short>(port));
		addr.sin_addr = reinterpret_cast<sockaddr_in*>(res->ai_addr)->sin_addr;
		freeaddrinfo(res);

		if (connect(s->sock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR)
		{
			int err = WSAGetLastError();
			lua_pushnil(L);
			if (err == WSAECONNREFUSED) lua_pushstring(L, "connection refused");
			else if (err == WSAETIMEDOUT) lua_pushstring(L, "timeout");
			else lua_pushstring(L, "connection failed");
			return 2;
		}
		lua_pushinteger(L, 1);
		return 1;
	}

	static int l_send(lua_State* L)
	{
		LuaTcpSocket* s = CheckTcpSocket(L);
		size_t len = 0;
		const char* data = luaL_checklstring(L, 2, &len);
		int total = 0;
		while (total < static_cast<int>(len))
		{
			int n = send(s->sock, data + total, static_cast<int>(len - total), 0);
			if (n <= 0)
			{
				lua_pushnil(L);
				lua_pushstring(L, "closed");
				return 2;
			}
			total += n;
		}
		lua_pushinteger(L, total);
		return 1;
	}

	static int l_receive(lua_State* L)
	{
		LuaTcpSocket* s = CheckTcpSocket(L);
		const char* mode = luaL_optstring(L, 2, "*l");
		if (strcmp(mode, "*l") != 0)
		{
			lua_pushnil(L);
			lua_pushstring(L, "unsupported receive mode");
			return 2;
		}
		char buf[8192];
		int total = 0;
		while (true)
		{
			char ch;
			int n = recv(s->sock, &ch, 1, 0);
			if (n <= 0)
			{
				lua_pushnil(L);
				lua_pushstring(L, "closed");
				return 2;
			}
			if (ch == '\n') break;
			if (ch == '\r') continue;
			if (total < static_cast<int>(sizeof(buf) - 1)) buf[total++] = ch;
		}
		buf[total] = '\0';
		lua_pushstring(L, buf);
		return 1;
	}

	static int l_close(lua_State* L)
	{
		LuaTcpSocket* s = CheckTcpSocket(L);
		if (s->sock != INVALID_SOCKET)
		{
			closesocket(s->sock);
			s->sock = INVALID_SOCKET;
		}
		return 0;
	}

	static const luaL_Reg gTcpMethods[] =
	{
		{ "connect", l_connect },
		{ "send", l_send },
		{ "receive", l_receive },
		{ "settimeout", l_settimeout },
		{ "close", l_close },
		{ nullptr, nullptr }
	};

	static int luaopen_socket_core(lua_State* L)
	{
		luaL_newmetatable(L, SOCKET_CORE_TCP);
		luaL_setfuncs(L, gTcpMethods, 0);
		lua_pushvalue(L, -1);
		lua_setfield(L, -2, "__index");

		lua_newtable(L);
		lua_pushcfunction(L, l_tcp);
		lua_setfield(L, -2, "tcp");
		return 1;
	}

	// 拉起 LuaPanda。VSCode 未连接时 LuaPanda 会自动重试，不会阻塞游戏运行。
	static void StartLuaPanda(lua_State* L)
	{
		lua_getglobal(L, "LuaPanda");
		if (!lua_istable(L, -1))
		{
			lua_pop(L, 1);
			OutputDebugStringA("[LuaPanda] 全局 LuaPanda 不存在，调试器未启动（LuaPanda.lua 没加载成功）。\n");
			return;
		}
		lua_getfield(L, -1, "start");
		if (lua_isfunction(L, -1))
		{
			lua_pushstring(L, "127.0.0.1");
			lua_pushinteger(L, 8818);
			if (lua_pcall(L, 2, 0, 0) == LUA_OK)
			{
				OutputDebugStringA("[LuaPanda] start() 已调用，正在连接 VSCode(8818)...\n");
			}
			else
			{
				const char* e = lua_tostring(L, -1);
				OutputDebugStringA(("[LuaPanda] start() 出错: " + std::string(e ? e : "") + "\n").c_str());
				lua_pop(L, 1);
			}
		}
		else
		{
			lua_pop(L, 1);
		}
		lua_pop(L, 1);
	}

	// 加载 LuaPanda.lua：先走 package.path（依赖 CWD=Run），失败则从 exe 目录向上找 Run/Script/LuaPanda.lua
	static bool LoadLuaPanda(lua_State* L)
	{
		lua_getglobal(L, "require");
		lua_pushstring(L, "LuaPanda");
		if (lua_pcall(L, 1, 1, 0) == LUA_OK)
		{
			lua_pop(L, 1);
			return true;
		}
		lua_pop(L, 1); // require 报错信息

		char exePath[MAX_PATH] = { 0 };
		GetModuleFileNameA(nullptr, exePath, MAX_PATH);
		std::string dir = exePath;
		size_t p = dir.find_last_of("\\/");
		if (p != std::string::npos) dir = dir.substr(0, p);

		std::string d = dir;
		for (int i = 0; i < 6; ++i)
		{
			std::string tryA = d + "/Run/Script/LuaPanda.lua";
			std::string tryB = d + "/Script/LuaPanda.lua";
			if (luaL_dofile(L, tryA.c_str()) == LUA_OK) { lua_pop(L, 1); return true; }
			lua_pop(L, 1);
			if (luaL_dofile(L, tryB.c_str()) == LUA_OK) { lua_pop(L, 1); return true; }
			lua_pop(L, 1);
			p = d.find_last_of("\\/");
			if (p == std::string::npos) break;
			d = d.substr(0, p);
		}
		return false;
	}

	// 注册 socket.core 并拉起 LuaPanda；整段保护，缺插件/路径不对只打日志，不影响游戏运行。
	static void SetupLuaPandaDebug(lua_State* L)
	{
		luaL_requiref(L, "socket.core", luaopen_socket_core, 1);
		lua_pop(L, 1);

		if (!LoadLuaPanda(L))
		{
			OutputDebugStringA("[LuaPanda] 找不到 LuaPanda.lua（package.path 与 exe 回溯均未命中）。\n");
			return;
		}
		StartLuaPanda(L);
	}
}

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

		//SetupLuaPandaDebug(mpLuaState);
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
