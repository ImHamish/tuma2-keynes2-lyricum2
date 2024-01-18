#pragma once
#if defined(__cplusplus)
extern "C" {
#endif

#include "../liblua/lua.h"
#include "../liblua/lauxlib.h"
#include "../liblua/lualib.h"

#if LUA_V == 523
	#define luaL_reg		luaL_Reg

	#define lua_dobuffer	luaL_loadbuffer
	#define lua_dofile		luaL_dofile
	#define lua_dostring	luaL_dostring
	#define luaL_getn		lua_rawlen
	#define lua_resume(a,b)	lua_resume(a,0,b)


	#define lua_ref(L,lock)	((lock) ? luaL_ref(L, LUA_REGISTRYINDEX) : \
	  (lua_pushstring(L, "unlocked references are obsolete"), lua_error(L), 0))
	#define lua_unref(L,ref)	luaL_unref(L, LUA_REGISTRYINDEX, (ref))
#endif


#if defined(__cplusplus)
}
#endif
