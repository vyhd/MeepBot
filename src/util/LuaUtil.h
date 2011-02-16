#ifndef LUA_UTIL_H
#define LUA_UTIL_H

struct lua_State;

namespace LuaUtil
{
	lua_State* Open();
	void Close( lua_State *L );

	/* returns how many objects were removed from the stack */
	int CleanStack( lua_State *L );

	/* runs all the .lua scripts in a given directory */
	bool RunScriptsFromDir( lua_State *L, const char *path );
}

#endif // LUA_UTIL_H
