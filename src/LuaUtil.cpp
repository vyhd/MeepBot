#include <cstdio>
#include <lua5.1/lua.hpp>
#include <dirent.h>

#include "StringUtil.h"
#include "LuaUtil.h"

lua_State* LuaUtil::Open()
{
	lua_State *L = lua_open();

	if( L == NULL )
	{
		printf( "Failed to allocate lua_State!\n" );
		return L;
	}

	luaopen_base( L );
	luaopen_table( L );
	luaopen_string( L );
	luaopen_math( L );

	/* the above calls leave stuff on the stack, so we clean it */
	CleanStack( L );

	return L;
}

void LuaUtil::Close( lua_State *L )
{
	lua_close( L );
}

int LuaUtil::CleanStack( lua_State *L )
{
	int ret = lua_gettop(L);

	/* pop objects until we have a clean stack */
	while( lua_gettop(L) != 0 )
		lua_pop(L, 1);

	return ret;
}

bool LuaUtil::RunScriptsFromDir( lua_State *L, const char *path )
{
	DIR *dir = opendir( path );

	if( dir == NULL )
	{
		printf( "does not exist\n" );
		return false;
	}

	struct dirent *ent;

	while( true )
	{
		ent = readdir( dir );

		if( !ent )
			break;

		/* skip hidden and special files */
		if( ent->d_name[0] == '.' )
			continue;

		printf( "running file '%s'...\n", ent->d_name );

		/* we need to send the full relative path */
		std::string file = StringUtil::Format( "%s/%s", path, ent->d_name );

		if( luaL_dofile(L, file.c_str()) != 0 )
		{
			printf( "failed: %s\n", lua_tostring( L, -1 ) );
			closedir( dir );
			return false;
		}
	}

	closedir( dir );
	return true;
}

