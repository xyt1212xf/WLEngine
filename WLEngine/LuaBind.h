#pragma once
#include "ParseParam.h"
#include "ScriptEntity.h"
namespace WL
{
	template<typename Ret, typename Func, Func func, typename... Args, std::size_t... I>
	static int CallHelper(lua_State* pState, const std::index_sequence<I...>& )
	{
		Ret ret = func(CParseParam<Args>::parseParam(pState, I + 1)...);  // Lua 参数从索引 1 开始
		parseRet(pState, ret);  
		return 1;
	}

	template<typename Func, Func func, typename... Args, std::size_t... I>
	static void CallHelper(lua_State* pState, const std::index_sequence<I...>& )
	{
		func(CParseParam<Args>::parseParam(pState, I + 1)...);  // Lua 参数从索引 1 开始
	}

	template<typename Ret, typename Func, Func func, typename... Args, std::size_t... I>
	static int CallHelperT(lua_State* pState, const std::index_sequence<I...>&)
	{
		Ret ret = func(CParseParam<Args>::parseParam(pState, I + 1)...);  // Lua 参数从索引 1 开始
		parseRet(pState, ret);  // 解析返回值
		return 1;
	}

	template<typename Func, Func func, typename... Args, std::size_t... I>
	static void CallHelperT(lua_State* pState, const std::index_sequence<I...>&)
	{
		func(CParseParam<Args>::parseParam(pState, I + 1)...);  // Lua 参数从索引 1 开始
	}

	template<typename T, typename Ret, typename Func, Func func, typename... Args, std::size_t... I>
	static int CallMemHelperT(lua_State* pState, const std::index_sequence<I...>&)
	{
		lua_pushnumber(pState, 0);
		lua_gettable(pState, 1);
		T** ppobjct = (T**)lua_touserdata(pState, -1);
		Ret ret = ((*ppobjct)->*(func))(CParseParam<Args>::parseParam(pState, I + 2)...);  // Lua 参数从索引 2 开始
		parseRet(pState, ret);  // 解析返回值
		return 1;
	}

	template<typename T, typename Func, Func func, typename... Args, std::size_t... I>
	static void CallMemHelperT(lua_State* pState, const std::index_sequence<I...>&)
	{
		lua_pushnumber(pState, 0);
		lua_gettable(pState, 1);
		T** ppobjct = (T**)lua_touserdata(pState, -1);
		((*ppobjct)->*(func))(CParseParam<Args>::parseParam(pState, I + 2)...);  // Lua 参数从索引 2 开始
	}

	template<typename Ret>
	class CCallFun
	{
	public:
		template<typename Func, Func func, typename... Args>
		static typename std::enable_if<sizeof...(Args) == 0, int>::type CallFun(lua_State* pState)
		{
			func();
			return 1;
		}

		template<typename Func, Func func, typename... Args>
		static typename std::enable_if < sizeof...(Args) != 0, int>::type CallFun(lua_State* pState)
		{
			CallHelper<Ret, Func, func, Args...>(pState, std::index_sequence_for<Args...>{});
			return 1;
		}

		template<typename T, typename Func, Func func, typename... Args>
		static typename std::enable_if<sizeof...(Args) == 0, int>::type CallMemFun(lua_State* pState)
		{
			CallMemHelperT<T, Ret, Func, func, Args...>(pState, std::index_sequence_for<Args...>{});
			return 1;
		}

		template<typename T, typename Func, Func func, typename... Args>
		static typename std::enable_if < sizeof...(Args) != 0, int>::type CallMemFun(lua_State* pState)
		{
			CallMemHelperT<T, Ret, Func, func, Args...>(pState, std::index_sequence_for<Args...>{});
			return 1;
		}

	};

	template<>
	class CCallFun<void>
	{
	public:
		template<typename Func, Func func, typename... Args>
		static typename std::enable_if<sizeof...(Args) == 0, int>::type CallFun(lua_State* pState)
		{
			func();
			return 1;
		}

		template<typename Func, Func func, typename... Args>
		static typename std::enable_if < sizeof...(Args) != 0, int>::type CallFun(lua_State* pState)
		{
			CallHelper<Func, func, Args...>(pState, std::index_sequence_for<Args...>{});
			return 1;
		}

		template<typename T, typename Func, Func func, typename... Args>
		static typename std::enable_if<sizeof...(Args) == 0, int>::type CallMemFun(lua_State* pState)
		{
			CallMemHelperT<T, Func, func, Args...>(pState, std::index_sequence_for<Args...>{});
			return 1;
		}

		template<typename T, typename Func, Func func, typename... Args>
		static typename std::enable_if < sizeof...(Args) != 0, int>::type CallMemFun(lua_State* pState)
		{
			CallMemHelperT<T, Func, func, Args...>(pState, std::index_sequence_for<Args...>{});
			return 1;
		}
	};


	template<typename T>
	class CRegisterClass
	{
	public:
		CRegisterClass() = delete;
		~CRegisterClass() = delete;
		static int registerClass(lua_State* pLua)
		{
			static_assert(std::is_base_of<CScriptEntity, T>::value, "The T needs to inherit from the CScriptClass!");
			luaL_getmetatable(pLua, T::MetaName);
			if (lua_isnil(pLua, -1))
			{
				auto gc_obj = [](lua_State* pLuaState)->int
					{
						T** obj = static_cast<T**>(luaL_checkudata(pLuaState, -1, T::MetaName));
						if (nullptr != obj)
						{
							WL_DECREASE((*obj));
						}
						return 1;
					};

				auto _CreateClassObject = [](lua_State* pLuaState)->int
					{
						lua_newtable(pLuaState);

						lua_pushstring(pLuaState, "__index");
						luaL_getmetatable(pLuaState, T::MetaName);
						lua_rawset(pLuaState, -3);

						lua_pushinteger(pLuaState, 0);
						T** pUserData = (T**)lua_newuserdata(pLuaState, sizeof(T*));
						*pUserData = WL_NEW(T, Entity);
						luaL_getmetatable(pLuaState, T::MetaName);
						lua_setmetatable(pLuaState, -2);
						lua_rawset(pLuaState, -3);

						//lua_pushstring(L, "__newindex");
						//lua_pushcfunction(L, newindex);
						//lua_settable(L, -3);

						luaL_getmetatable(pLuaState, T::MetaName);
						lua_setmetatable(pLuaState, -2);
						printf("ClassName = %s\n", T::MetaName);
						return 1;
					};


				luaL_newmetatable(pLua, T::MetaName);
				lua_pushstring(pLua, "__index");
				lua_pushvalue(pLua, -2);
				lua_settable(pLua, -3);

				lua_pushstring(pLua, "__gc");
				lua_pushcfunction(pLua, gc_obj);
				lua_settable(pLua, -3);

				//////////////////////////////////////////////////////////////////////////
				lua_newtable(pLua);
				lua_pushstring(pLua, "New");
				lua_pushcfunction(pLua, _CreateClassObject);
				lua_settable(pLua, -3);

				lua_setglobal(pLua, T::ClassName);

				return 0;
			}
			return 1;
		}
	};

	template<typename Ret>
	class CRegisterFun
	{
	public:
		CRegisterFun() = default;
		~CRegisterFun() = default;

		template<typename Fun, Fun func, typename... Args>
		static void registrFun(lua_State* pLua, const std::string& szFunName)
		{
			static_assert(std::is_invocable_v<Fun, Args...>, "Func must be invocable!");
			lua_CFunction pFun = &CCallFun<Ret>::template CallFun<Fun, func, Args...>;
			lua_register(pLua, szFunName.c_str(), pFun);
		}

		template<typename T, typename Fun, Fun func, typename... Args>
		static void registerClassMemberFun(lua_State* pLua, const std::string& funName)
		{
			int top = lua_gettop(pLua);
			luaL_getmetatable(pLua, T::MetaName);
			if (!lua_isnil(pLua, -1))
			{
				lua_pushstring(pLua, funName.c_str());
				lua_CFunction pFun = &CCallFun<Ret>::template CallMemFun<T, Fun, func, Args...>;
				lua_pushcfunction(pLua, pFun);
				lua_rawset(pLua, -3);
			}
			lua_settop(pLua, top);
		}
	};

	template<>
	class CRegisterFun<void>
	{
	public:
		CRegisterFun() = default;
		~CRegisterFun() = default;

		template<typename Fun, Fun func, typename... Args>
		static void registrFun(lua_State* pLua, const std::string& szFunName)
		{
			static_assert(std::is_invocable_v<Fun, Args...>, "Func must be invocable!");
			lua_CFunction pFun = &CCallFun<void>::template CallFun<Fun, func, Args...>;
			lua_register(pLua, szFunName.c_str(), pFun);
		}

		template<typename T, typename Fun, Fun func, typename... Args>
		static void registerClassMemberFun(lua_State* pLua, const std::string& funName)
		{
			int top = lua_gettop(pLua);
			luaL_getmetatable(pLua, T::MetaName);
			if (!lua_isnil(pLua, -1))
			{
				lua_pushstring(pLua, funName.c_str());
				lua_CFunction pFun = &CCallFun<void>::template CallMemFun<T, Fun, func, Args...>;
				lua_pushcfunction(pLua, pFun);
				lua_rawset(pLua, -3);
			}
			lua_settop(pLua, top);
		}
	};
}


////类成员函数
//#define REGISTER_BASECLASS_MEMFUN0( C, B, R, N, F ) \
//	RegMemFun<R, C, &B::F>( N );
//
//#define REGISTER_CLASS_MEMFUN0( C, R, N, F ) \
//	RegMemFun<R, C, &C::F>( N );
//
//#define REGISTER_CLASS_MEMFUN1( C, R, P1, N, F ) \
//	RegMemFun<R, P1, C, &C::F>( N );
//
//#define REGISTER_CLASS_MEMFUN2( C, R, P1, P2, N, F ) \
//	RegMemFun<R, P1, P2, C, &C::F>( N );
//
//#define REGISTER_CLASS_MEMFUN3( C, R, P1, P2, P3, N, F ) \
//	RegMemFun<R, P1, P2, P3, C, &C::F>( N );
//
//#define REGISTER_CLASS_MEMFUN4( C, R, P1, P2, P3, P4, N, F ) \
//	RegMemFun<R, P1, P2, P3, P4, C, &C::F>( N );
//
//#define REGISTER_CLASS_MEMFUN5( C, R, P1, P2, P3, P4, P5, N, F ) \
//	RegMemFun<R, P1, P2, P3, P4, P5, C, &C::F>( N );
//
//#define REGISTER_CLASS_MEMFUN6( C, R, P1, P2, P3, P4, P5, P6, N, F ) \
//	RegMemFun<R, P1, P2, P3, P4, P5, P6, C, &C::F>( N );
//
//
////类静态成员函数
//#define REGISTER_CLASS_STATIC_MEMFUN0( R, F, N ) \
//	RegFun< R, R(*)(), &F>( #N );
//
//#define REGISTER_CLASS_STATIC_MEMFUN1( R, P1, F, N ) \
//	RegFun< R, C0, R(*)(C0), &F>( #N );
//
//#define REGISTER_CLASS_STATIC_MEMFUN2( R, P1, P2, F, N ) \
//	RegFun< R, C0, C1, R(*)(C0, C1), &F>( #N );
//
//#define REGISTER_CLASS_STATIC_MEMFUN3( R, P1, P2, P3, F, N ) \
//	RegFun< R, C0, C1, C2, R(*)(C0, C1, C2), &F>( #N );
//
//#define REGISTER_CLASS_STATIC_MEMFUN4( R, P1, P2, P3, P4, F, N ) \
//	RegFun< R, C0, C1, C2, C3, R(*)(C0, C1, C2, C3), &F>( #N );
//
//#define REGISTER_CLASS_STATIC_MEMFUN5( R, P1, P2, P3, P4, P5, F, N ) \
//	RegFun< R, C0, C1, C2, C3, C4, R(*)(C0, C1, C2, C3, C4), &F>( #N );
//
//#define REGISTER_CLASS_STATIC_MEMFUN6( R, P1, P2, P3, P4, P5, P6, F, N ) \
//	RegFun< R, C0, C1, C2, C3, C4, C5, R(*)(C0, C1, C2, C3, C4, C5), &F>( #N );
//
//
//#include "CClass.h"


	

