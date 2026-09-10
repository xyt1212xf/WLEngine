#pragma once
#include "Common.h"
#include "lua.hpp"
#include "ScriptEntity.h"
#include <iostream>
namespace WL 
{
	enum ETypeEnum
	{
		eTypeNone = 0,
		eTypeBool,
		eTypeChar,
		eTypeUChar,
		eTypeShort,
		eTypeUShort,
		eTypeInt,
		eTypeUInt,
		eTypeLong,
		eTypeULong,
		eTypeDouble,
		eTypeFloat,
		eTypeString,
		eTypeUseData,
	};
	template<typename T>
	class CType2Enum
	{
	public:
		enum {
			Value = eTypeNone,
		};
	};
	template<>
	class CType2Enum<bool>
	{
	public:
		enum {
			Value = eTypeBool,
		};
	};

	template<>
	class CType2Enum<char>
	{
	public:
		enum {
			Value = eTypeChar,
		};
	};

	template<>
	class CType2Enum<unsigned char>
	{
	public:
		enum {
			Value = eTypeUChar,
		};
	};

	template<>
	class CType2Enum<short>
	{
	public:
		enum {
			Value = eTypeShort,
		};
	};

	template<>
	class CType2Enum<unsigned short>
	{
	public:
		enum {
			Value = eTypeUShort,
		};
	};

	template<>
	class CType2Enum<int>
	{
	public:
		enum {
			Value = eTypeInt,
		};
	};

	template<>
	class CType2Enum<unsigned int>
	{
	public:
		enum {
			Value = eTypeUInt,
		};
	};

	template<>
	class CType2Enum<INT64>
	{
	public:
		enum {
			Value = eTypeLong,
		};
	};

	template<>
	class CType2Enum<UINT64>
	{
	public:
		enum {
			Value = eTypeULong,
		};
	};

	template<>
	class CType2Enum<float>
	{
	public:
		enum {
			Value = eTypeFloat,
		};
	};

	template<>
	class CType2Enum<double>
	{
	public:
		enum {
			Value = eTypeDouble,
		};
	};

	template<>
	class CType2Enum<char*>
	{
	public:
		enum {
			Value = eTypeString,
		};
	};

	template<>
	class CType2Enum<const char*>
	{
	public:
		enum {
			Value = eTypeString,
		};
	};

	template<>
	class CType2Enum<CScriptEntity*>
	{
	public:
		enum {
			Value = eTypeUseData,
		};
	};

	//////////////////////////////////////////////////////////////////////////
	//数据类型 

	template<typename T>
	class CParseParam
	{
	public:
		static T parseParam(lua_State* pState, int index)
		{
			int type = CType2Enum<T>::Value;
			switch (type)
			{
			case eTypeChar:
			case eTypeUChar:
			case eTypeShort:
			case eTypeUShort:
			case eTypeInt:
			case eTypeUInt:
			case eTypeFloat:
			case eTypeDouble:
				return static_cast<T>(lua_tonumber(pState, index));
			case eTypeLong:
			case eTypeULong:
				return static_cast<T>(lua_tonumber(pState, index));
			default:
				printf("不正确的参数类型");
				break;
			}

			return 0;
		}
	};

	template<>
	class CParseParam<char*>
	{
	public:
		static char* parseParam(lua_State* pState, int index)
		{
			return const_cast<char*>(lua_tostring(pState, index));
		}
	};

	template<>
	class CParseParam<const char*>
	{
	public:
		static const char* parseParam(lua_State* pState, int index)
		{
			return lua_tostring(pState, index);
		}
	};

	template<>
	class CParseParam<std::string&>
	{
	public:
		static char* parseParam(lua_State* pState, int index)
		{
			return const_cast<char*>(lua_tostring(pState, index));
		}
	};

	template<>
	class CParseParam<const std::string&>
	{
	public:
		static const char* parseParam(lua_State* pState, int index)
		{
			return lua_tostring(pState, index);
		}
	};

	template<>
	class CParseParam<bool>
	{
	public:
		static bool parseParam(lua_State* pState, int index)
		{
			return lua_toboolean(pState, index) != 0 ? true : false;
		}
	};

	template<>
	class CParseParam<CScriptEntity*>
	{
	public:
		static CScriptEntity* parseParam(lua_State* pState, int index)
		{
#ifdef DEBUT_TEST
			// 确保索引是表
			if (!lua_istable(pState, index)) {
				std::cerr << "The given index is not a table!" << std::endl;
				return nullptr;
			}

			// 确保索引为绝对索引
			if (index < 0) 
			{
				index = lua_gettop(pState) + index + 1;
			}

			lua_pushnil(pState);  // 首先将 nil 压栈，作为遍历的起点

			std::cout << "Lua Table Contents:" << std::endl;
			while (lua_next(pState, index) != 0) 
			{
				// 键在栈位置 -2，值在栈位置 -1

				// 获取键和值
				std::string key;
				if (lua_type(pState, -2) == LUA_TSTRING) {
					key = lua_tostring(pState, -2);
				}
				else if (lua_type(pState, -2) == LUA_TNUMBER) {
					key = std::to_string(lua_tonumber(pState, -2));
				}
				else {
					key = "[unknown key type]";
				}

				std::string value;
				if (lua_type(pState, -1) == LUA_TSTRING) {
					value = lua_tostring(pState, -1);
				}
				else if (lua_type(pState, -1) == LUA_TNUMBER) {
					value = std::to_string(lua_tonumber(pState, -1));
				}
				else if (lua_type(pState, -1) == LUA_TBOOLEAN) {
					value = lua_toboolean(pState, -1) ? "true" : "false";
				}
				else if (lua_type(pState, -1) == LUA_TTABLE) {
					value = "{table}";
				}
				else if (lua_type(pState, -1) == LUA_TUSERDATA) {
					value = "{usedata}";
				}
				else {
					value = "[unknown value type]";
				}

				std::cout << key << " = " << value << std::endl;

				// 移除值，保留键作为下一次遍历的起点
				lua_pop(pState, 1);
			}
#endif
			CScriptEntity* pEntity = nullptr;
			if (lua_istable(pState, index))
			{
				lua_pushvalue(pState, index);
				lua_pushinteger(pState, 0);
				lua_gettable(pState, -2);
				CScriptEntity** pUseData = static_cast<CScriptEntity**>(lua_touserdata(pState, -1));
				pEntity = *pUseData;
			}
			return pEntity;
		}
	};


	//static CScriptEntity* parseParam(lua_State* pState, int index)
	//{
	//	CScriptEntity* pEntity = nullptr;
	//	if (lua_istable(pState, index))
	//	{
	//		lua_pushvalue(pState, index);


	//		lua_pushinteger(pState, 0);
	//		lua_gettable(pState, -2);

	//		auto pUseData = lua_touserdata(pState, -1);
	//		pEntity = static_cast<CScriptEntity*>(pUseData);
	//	}
	//	return pEntity;
	//}

	template<typename T>
	static void parseRet(lua_State* pState, T ret)
	{
		int type = CType2Enum<T>::Value;
		switch (type)
		{
		case eTypeChar:
		case eTypeUChar:
		case eTypeShort:
		case eTypeUShort:
		case eTypeInt:
		case eTypeUInt:
		case eTypeFloat:
		case eTypeDouble:
			lua_pushnumber(pState, static_cast<double>(ret));
			break;
		case eTypeLong:
		case eTypeULong:
			//notify It won't reach the upper limit of UINT64, it's just a double type of data. !!!!!
			lua_pushnumber(pState, static_cast<double>(ret));
			break;
		default:
			printf("不正确的参数类型");
			break;
		}
	}

	template<>
	[[maybe_unused]]
	void parseRet(lua_State* pState, char* ret)
	{
		lua_pushstring(pState, ret);
	}


	template<>
	[[maybe_unused]]
	void parseRet(lua_State* pState, const char* ret)
	{
		lua_pushstring(pState, ret);
	}

	template<>
	[[maybe_unused]]
	void parseRet(lua_State* pState, std::string ret)
	{
		lua_pushstring(pState, ret.c_str());
	}

	template<>
	[[maybe_unused]]
	void parseRet(lua_State* pState, const std::string& ret)
	{
		lua_pushstring(pState, ret.c_str());
	}

	template<>
	[[maybe_unused]]
	void parseRet(lua_State* pState, bool ret)
	{
		lua_pushboolean(pState, ret);
	}



	//template<>
	//static void ParseRet( lua_State* pState, ResourceBase* pWnd )
	//{
	//	
	//}

	//////////////////////////////////////////////////////////////////////////
	//
}