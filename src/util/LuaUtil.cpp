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
	struct dirent **dirlist;
	int results = scandir( path, &dirlist, NULL, alphasort );

	if( results < 0 )
	{
		printf( "\n" );
		return false;
	}

	bool bRet = true;

	for( int i = 0; i < results; ++i )
	{
		const char *name = dirlist[i]->d_name;

		/* skip hidden and special files */
		if( name[0] == '.' )
			continue;

		/* skip anything that isn't a Lua script */
		unsigned len = strlen(name);
		const char *ext = name + len - 3;
 
		if( StringUtil::CompareNoCase(ext, "lua") != 0 )
		{
			printf( "%s not Lua script (%s), not running\n", name, ext );
			continue;
		}

		printf( "running file '%s'...\n", name );

		/* we need to send the full relative path */
		std::string file = StringUtil::Format( "%s/%s", path, name );

		if( luaL_dofile(L, file.c_str()) != 0 )
		{
			printf( "failed: %s\n", lua_tostring( L, -1 ) );
			bRet = false;
			break;
		}
	}

	for( int i = 0; i < results; ++i )
		free( dirlist[i] );

	free( dirlist );

	return bRet;
}

