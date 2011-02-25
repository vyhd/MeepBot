/* This file contains all the Lua bindings for the bot as well as
 * the registration mechanism that creates the MeepBot table et al. */

#include <cstring>
#include <lua5.1/lua.hpp>
#include "MeepBot.h"
#include "MeepBot_LuaBindings.h"
#include "util/ISAAC.h"
#include "util/LuaUtil.h"
#include "util/StringUtil.h"
#include "database/QuotesDB.h"
#include "database/UserDB.h"
#include "packet/MessageCodes.h"

#include <sys/time.h>	/* for gettimeofday */

#include <string>
using namespace std;

/* simple helper functions */

void SetKeyVal( lua_State *L, const char *key, int val, int idx = -3 )
{
	lua_pushstring( L, key );
	lua_pushnumber( L, val );
	lua_settable( L, idx );
} 

void SetKeyVal( lua_State *L, const char *key, const char *val, int idx = -3 )
{
	lua_pushstring( L, key );
	lua_pushstring( L, val );
	lua_settable( L, idx );
}

void SetKeyVal( lua_State *L, const char *key, bool val, int idx = -3 )
{
	lua_pushstring( L, key );
	lua_pushboolean( L, int(val) );
	lua_settable( L, idx );
}

/* sets a given key to nil. */
void SetKeyNil( lua_State *L, const char *key, int idx = -3 )
{
	lua_pushstring( L, key );
	lua_pushnil( L );
	lua_settable( L, idx );
}

struct PacketType { MessageCode value; const char *name; };

const struct PacketType LuaPacketTypes[] =
{
	{ ROOM_MESSAGE, "TYPE_CHAT" },
	{ USER_PM,	"TYPE_PM" },
	{ MOD_CHAT,	"TYPE_MOD_CHAT" },

	/* arbitrary value, iteration sentinel */
	{ USER_JOIN,	NULL }
};

struct AccessType { AccessLevel value; const char *name; };

const struct AccessType LuaAccessTypes[] =
{
	{ LEVEL_BANNED,	"LEVEL_BANNED" },
	{ LEVEL_USER,	"LEVEL_USER" },
	{ LEVEL_OP,	"LEVEL_OP" },
	{ LEVEL_MOD,	"LEVEL_MOD" },
	{ LEVEL_ADMIN,	"LEVEL_ADMIN" },

	/* arbitrary value, interation sentinel */
	{ LEVEL_BANNED,	NULL }
};

/* Registers a list of enumerations into the global namespace, using a list of
 * structs with enum "value" and const char* "name" respectively. Setting
 * "name" to NULL acts as an iteration-ending sentinel. */
template<class T>
static void RegisterTypeGlobals( lua_State *L, const T *array )
{
	for( unsigned i = 0; true; ++i )
	{
		const T &e = array[i];

		if( e.name == NULL )
			break;

		lua_pushnumber( L, e.value );
		lua_setfield( L, LUA_GLOBALSINDEX, e.name );

		printf( "registered %d -> %s\n", e.value, e.name );
	}
}

static int Say( lua_State *L )
{
	if( lua_isstring(L, -1) )
		BOT->Say( lua_tostring(L, -1) );

	return 0;
}

static int Emote( lua_State *L )
{
	if( lua_isstring(L, -1) )
		BOT->Emote( lua_tostring(L, -1) );

	return 0;
}

static int PM( lua_State *L )
{
	const char *name = lua_tostring( L, -2 );
	const char *msg = lua_tostring( L, -1 );

	if( name == NULL || msg == NULL )
		return 0;

	BOT->PM( name, msg );
	return 0;
}

static int Rand( lua_State *L )
{
	unsigned ret = BOT->m_pISAAC->GetValue();

	/* optional parameter for maximum value */
	if( lua_isnumber(L, -1) )
		ret %= unsigned( lua_tonumber(L, -1) );

	// Lua is one indexed, so offset from 0..n-1 to 1..n
	lua_pushnumber( L, ret + 1 );

	return 1;
}

static int Resolve( lua_State *L )
{
	const char *caller = lua_tostring( L, -2 );
	const char *pattern = lua_tostring( L, -1 );

	if( caller != NULL && pattern != NULL )
		lua_pushstring( L, BOT->m_UserList.Resolve(caller, pattern).c_str() );
	else
		lua_pushnil( L );

	return 1;
}

static int QuoteAdd( lua_State *L )
{
	const char *adder = lua_tostring( L, -2 );
	const char *quote = lua_tostring( L, -1 );

	if( adder != NULL && quote != NULL )
		lua_pushboolean( L, BOT->m_pQuotesDB->AddQuote(adder, quote) );
	else
		lua_pushboolean( L, 0 );

	return 1;
}

static int QuoteGetRandomID( lua_State *L )
{
	int idx = BOT->m_pQuotesDB->GetRandomID();

	if( idx > 0 )
		lua_pushnumber( L, idx );
	else
		lua_pushnil( L );

	return 1;
}

static int QuoteGetIDByPattern( lua_State *L )
{
	if( !lua_isstring(L, -1) )
	{
		lua_pushnil( L );
		return 1;
	}

	int idx = BOT->m_pQuotesDB->GetIDByPattern( lua_tostring(L, -1) );

	if( idx > 0 )
		lua_pushnumber( L, idx );
	else
		lua_pushnil( L );

	return 1;
}

static int QuoteGetByID( lua_State *L )
{
	if( !lua_isnumber(L, -1) )
	{
		lua_pushnil( L );
		return 1;
	}

	int idx = int( lua_tonumber(L,-1) );

	/* GetQuote allocates 'quote' - lua_tostring copies, then we delete */
	{
		const char *quote = BOT->m_pQuotesDB->GetQuoteByID( idx );

		if( quote )
			lua_pushstring( L, quote );
		else
			lua_pushnil( L );

		delete[] quote;
	}

	return 1;
}

static int QuoteRemove( lua_State *L )
{
	if( !lua_isnumber(L, -1) )
	{
		lua_pushboolean( L, 0 );
		return 1;
	}

	/* currently, we don't have a way to tell if a quote was
	 * actually deleted. just return true if we didn't bork. */

	int idx = int( lua_tonumber(L,-1) );
	BOT->m_pQuotesDB->RemoveQuote( idx );

	lua_pushboolean( L, 1 );

	return true;
}

static int QuoteGetAuthor( lua_State *L )
{
	if( !lua_isnumber(L,-1) )
	{
		lua_pushnil( L );
		return 1;
	}

	int idx = (int)lua_tonumber(L, -1);

	/* 'author' is allocated in GetQuoteAuthor, we delete[] it here. */
	{
		const char *author = BOT->m_pQuotesDB->GetQuoteAuthor( idx );

		if( author )
			lua_pushstring( L, author );
		else
			lua_pushnil( L );

		delete[] author;
	}

	return 1;
}

static int GetAccessLevel( lua_State *L )
{
	const char *name = lua_tostring( L, -1 );

	if( name )
		lua_pushnumber( L, BOT->m_pUserDB->GetAccessLevel(name) );
	else
		lua_pushnil( L );

	return 1;
}

static int GetUserEntry( lua_State *L )
{
	const char *name = lua_tostring( L, -1 );

	UserEntry entry;

	if( name == NULL || !BOT->m_pUserDB->GetUserEntry(name, entry) )
	{
		lua_pushnil( L );
		return 1;
	}

	/* create a table */
	lua_newtable( L );

	/* push the UserEntry's key-value pairs to the table */
	SetKeyVal( L, "level", entry.level );
	SetKeyVal( L, "desc", entry.desc.c_str() );
	SetKeyVal( L, "protected", entry.locked );

	/* leave the table on the stack */
	return 1;
}

/* Placed under the core MeepBot table. */
static const luaL_reg bot_funcs[] =
{
	/* chat message functions */
	{ "Say",	Say },
	{ "Emote",	Emote },
	{ "PM",		PM },

#if 0
	/* moderator functions - use with care */
	{ "Kick",	Kick },
	{ "Disable",	Disable },
	{ "Ban",	Ban },
	{ "Unban",	Unban },
#endif

	{ NULL,		NULL },
};

/* Placed under the MeepBot.Util table. */
static const luaL_reg util_funcs[] =
{
	{ "Rand",	Rand },
	{ "Resolve",	Resolve },
	{ NULL,		NULL },
};

/* Placed under the MeepBot.Quotes table. */
static const luaL_reg quote_funcs[] =
{
	{ "Add",		QuoteAdd },
	{ "Remove",		QuoteRemove },
	{ "GetByID",		QuoteGetByID },
	{ "GetRandomID",	QuoteGetRandomID },
	{ "GetIDByPattern",	QuoteGetIDByPattern },
	{ "GetAuthor",		QuoteGetAuthor },
	{ NULL,			NULL }
};

#if 0
/* Placed under the MeepBot.Users table. */
static const luaL_reg user_funcs[] =
{
	{ "
#endif

/* temp timing stuff */
#include <sys/time.h>

uint64_t GetTimeElapsed( struct timeval &start, struct timeval &end )
{
	uint64_t ustart = start.tv_sec * 1000000 + start.tv_usec;
	uint64_t uend = end.tv_sec * 1000000 + end.tv_usec;
	return uend - ustart;
}

bool MeepBot_LuaBindings::Command( lua_State *L, int type, const char *cmd,
	const char *caller, const char *params )
{
	/* Note: all commands are lowercase at this point. */
	printf( "Command( %d, %s, %s, %s )\n", type, cmd, caller, params );

	/* HACK: if we get the 'reload' command, re-load all our scripts. */
	if( strcmp(cmd, "reload") == 0 )
	{
		bool bPM = type == USER_PM || StringUtil::CompareNoCase(caller, "Fire_Adept") != 0;

		struct timeval start, end;

		if( bPM )
			BOT->PM( caller, "Reloading Lua scripts..." );
		else
			BOT->Say( "Reloading Lua scripts..." );
		
		string err = "";
		gettimeofday( &start, NULL );
		bool ret = BOT->ReloadLua( err );
		gettimeofday( &end, NULL );

		string str;

		if( ret )
		{
			uint64_t microsec = GetTimeElapsed( start, end );
			str = StringUtil::Format( "...done. (took %llu usec)", microsec );
		}
		else
		{
			str = "...failed :(";

			if( type == USER_PM )
				str += " (" + err + ")";
		}

		if( bPM )
			BOT->PM( caller, str.c_str() );
		else
			BOT->Say( str.c_str() );

		return ret;
	}

	if( L == NULL )
	{
		BOT->Say( "The Lua broke :( Please fix it and !reload" );
		return false;
	}

	lua_getglobal( L, "MeepBot" );
	lua_pushstring( L, "Commands" );
	lua_gettable( L, -2 );

	if( !lua_istable(L, -1) )
	{
		BOT->Say( "Someone broke my Commands table, and I am most displeased." );
		BOT->CloseLua();
		return false;
	}

	lua_pushstring( L, cmd );
	lua_gettable( L, -2 );

	if( !lua_isfunction( L, -1) )
		return false;

	/* run this function with the given info */
	lua_pushnumber( L, type );
	lua_pushstring( L, caller );

	/* push the params, or nil if the params string is empty */
	if( strlen(params) > 0 )
		lua_pushstring( L, params );
	else
		lua_pushnil( L );

	/* Call the command with 3 arguments and 0 results */
	if( lua_pcall( L, 3, 0, 0 ) != 0 )
	{
		printf( "Failed to run command \"%s\": %s\n", cmd, lua_tostring(L, -1) );
		return false;
	}

	LuaUtil::CleanStack( L );
	return true;
}

/* UGLY HACK: luaL_register is behaving idiotically with objects on the
 * stack and we need to use it (or roll our own implementation, which I
 * don't want to do) to register commands under MeepBot, so we have a
 * wrapper function that will luaL_register into the global namespace,
 * making luaL_register happy, then reassign its reference into MeepBot. */

static void RegisterTable( lua_State *L, const char *name, const luaL_Reg *reg )
{
	assert( lua_gettop(L) == 0 );

	lua_getglobal( L, "MeepBot" );
	assert( lua_istable(L, -1) );

	lua_getfield( L, -1, name );
	assert( lua_isnil(L, -1) );
	lua_pop( L, -2 );

	if( reg == NULL )
	{
		/* push a new table into the global namespace, then pop */
		lua_pushstring( L, name );
		lua_newtable( L );
		lua_settable( L, LUA_GLOBALSINDEX );
	}
	else
	{
		/* register into global namespace, then pop the table */
		luaL_register( L, name, reg );
		lua_pop( L, -1 );

		assert( lua_gettop(L) == 0 );
	}

	/* push this table reference into MeepBot.<name> */
	lua_getglobal( L, "MeepBot" );
	lua_pushstring( L, name );
	lua_getglobal( L, name );
	lua_settable( L, -3 );

	/* pop the MeepBot table */
	lua_pop( L, -1 );

	/* kill the global reference */
	SetKeyNil( L, name, LUA_GLOBALSINDEX );

	assert( lua_gettop(L) == 0 );
}

void MeepBot_LuaBindings::Register( lua_State *L )
{
	/* push MeepBot to the stack */
	lua_getglobal( L, "MeepBot" );

	/* if it already exists, remove and rebuild */
	bool bIsNil = lua_isnil( L, -1 );
	lua_pop( L, -1 );

	if( !bIsNil )
		SetKeyNil( L, "MeepBot", LUA_GLOBALSINDEX );

	/* register MeepBot, then register its subtables */
	luaL_register( L, "MeepBot", bot_funcs );
	lua_pop( L, -1 );

	RegisterTable( L, "Utils", util_funcs );
	RegisterTable( L, "Quotes", quote_funcs );
	RegisterTable( L, "Commands", NULL );

	/* register a few important enums in the global namespace */
	RegisterTypeGlobals( L, LuaPacketTypes );
	RegisterTypeGlobals( L, LuaAccessTypes );
}
