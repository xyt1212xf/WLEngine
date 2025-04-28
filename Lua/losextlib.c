


#define losextlib_c
#define LUA_LIB

#include "lprefix.h"


#include <errno.h>
#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"


static int osext_getMilliseconds(lua_State* L) 
{
	return 1;
}


static int osext_getMicroseconds(lua_State* L) 
{
	return 1;
}


static const luaL_Reg syslib[] = {
  {"getMilliseconds",   osext_getMilliseconds},
  {"getMicroseconds",	osext_getMicroseconds},
  {NULL, NULL}
};

LUAMOD_API int luaopen_osExt(lua_State* L) {
	luaL_newlib(L, syslib);
	return 1;
}

