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

	/* sets t[key] = val, where t is the table at the given index. */
	void SetKeyVal( lua_State *L, const char *key, int val, int idx = -3 );
	void SetKeyVal( lua_State *L, const char *key, bool val, int idx = -3 );
	void SetKeyVal( lua_State *L, const char *key, const char *val, int idx = -3 );

	/* sets a given key to nil. */
	void SetKeyNil( lua_State *L, const char *key, int idx = -3 );

	/* registers enumerations into the global namespace. T must be a
	 * struct with enum "value" and char* "name". The list must end with
	 * an entry whose name is NULL (end-of-iteration sentinel). */
	template<class T>
	void RegisterGlobalEnum( lua_State *L, const T* array )
	{
		for( unsigned i = 0; true; ++i )
		{
			const T &e = array[i];
			if( e.name == NULL )
				break;
			lua_pushnumber( L, e.value );
			lua_setglobal( L, e.name );
		}
	}
}

#endif // LUA_UTIL_H
