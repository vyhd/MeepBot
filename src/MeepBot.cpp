#include "MeepBot.h"
#include "database/UserDB.h"
#include "packet/ChatPacket.h"
#include "packet/MessageCodes.h"
#include "packet/PacketUtil.h"
#include "util/ISAAC.h"
#include "util/LuaUtil.h"
#include "util/StringUtil.h"
#include <lua5.1/lua.hpp>

#include <ctime>
#include <cerrno>
#include <cstring>

using namespace std;

MeepBot *BOT = NULL;

/* directory from which Lua scripts are loaded */
const char* SCRIPTS_DIR = "scripts";

/* RGB values for the chat bot's messages */
const char RED = 92;
const char GREEN = 0;
const char BLUE = 168;

MeepBot::MeepBot()
{
	m_bLoggedIn = false;

	BOT = this;
	L = NULL;

	memset( m_sReadBuffer, 0, BUFFER_SIZE );

	/* lol I'm so clever :awesome: */
	unsigned int seed[8] = { 'M','E','E','P','B','O','T', time(NULL) };
	m_pISAAC = new ISAAC( seed );
}

MeepBot::~MeepBot()
{
	delete m_pISAAC;
}

void MeepBot::Say( const char *str )
{
	if( !m_bLoggedIn || strlen(str) == 0 )
		return;

	Write( ChatPacket(ROOM_MESSAGE, BLANK, str, RED, GREEN, BLUE) );
}

void MeepBot::Emote( const char *str )
{
	if( !m_bLoggedIn || strlen(str) == 0 )
		return;

	Write( ChatPacket(ROOM_ACTION, BLANK, str) );
}

void MeepBot::PM( const char *name, const char *msg )
{
	if( !m_bLoggedIn || strlen(msg) == 0 )
		return;

	/* TODO: verify that a user is actually on the server first */
	Write( ChatPacket(USER_PM, name, msg) );
}

bool MeepBot::OpenLua( std::string &err )
{
	/* initialize the internal Lua state and run all our scripts */
	L = LuaUtil::Open();

	Register( L );

	if( !LuaUtil::RunScriptsFromDir(L, SCRIPTS_DIR) )
	{
		err = lua_tostring( L, -1 );
		printf( "Parse error: %s\n", err.c_str() );
		CloseLua();
		return false;
	}

	return true;
}

void MeepBot::CloseLua()
{
	if( L == NULL )
		return;

	LuaUtil::Close( L );
	L = NULL;
}

bool MeepBot::ReloadLua( std::string &err )
{
	CloseLua();
	return OpenLua( err );
}

bool MeepBot::Connect( const char *host, unsigned port )
{
	return m_Socket.OpenHost(host, port);
}

bool MeepBot::Login( const char *name, const char *pwd, int retries )
{
	ChatPacket req = ChatPacket(USER_JOIN, name, pwd);

	while( retries > 0 )
	{
		/* send our login message, wait for the response */
		if( Write(req) > 0 && Read() )
		{
			int response = ChatPacket(m_sReadBuffer).iCode;

			if( response == ACCESS_GRANTED )
			{
				m_bLoggedIn = true;
				Say( "Greetings, Mortals." );

				/* request a list of currently chatting users
				 * - we'll pick this up in MainLoop. */
				Write( ChatPacket(USER_LIST) );

				return true;
			}

			printf( "Denied: code %d\n", response );
		}

		printf( "Failed to login to server.\n" );

		if( retries >= 0 )
		{
			printf( "Retrying in 5 seconds.\n" );
			sleep( 5 );
		}

		--retries;
	}

	printf( "Gave up connecting. Better luck next time.\n" );
	return false;
}

void MeepBot::Logout()
{
	if( m_bLoggedIn )
		Write( ChatPacket(USER_PART, BLANK, BLANK) );
}
		
void MeepBot::MainLoop()
{
	while( m_bLoggedIn )
	{
		/* this waits on network I/O, so we sleep between reads. */
		int len = Read();

		if( len == -1 )
			break;

		m_sReadBuffer[len] = '\0';

		if( PacketUtil::NeedsSplit(m_sReadBuffer) )
		{
			vector<string> packets;
			PacketUtil::Split( m_sReadBuffer, packets );

			for( unsigned i = 0; i < packets.size(); i++ )
				HandlePacket( packets[i].c_str() );
		}
		else
		{
			HandlePacket( m_sReadBuffer );
		}
	}
}

void MeepBot::HandlePacket( const char *data )
{
	ChatPacket packet( data );

	/* references to the above, for convenience */
	MessageCode code = MessageCode(packet.iCode);
	const string &user = packet.sUsername;
	const string &msg = packet.sMessage;

	switch( code )
	{
	case USER_LIST:
		if( user == BLANK )
			break;

		/* fall through */
	case USER_JOIN:
		m_UserList.Add( user );
		break;
	case USER_PART:
		m_UserList.Remove( user );
		break;
	case ROOM_MESSAGE:
	case USER_PM:
		/* ignore commands from ourself */
		if( user == "MeepBot" )
			break;

		if( code == USER_PM )
			printf( "[%s] %s\n", user.c_str(), msg.c_str() );

		/* Handle commands from external users */
		if( msg[0] == '!' )
		{
			const size_t pos = msg.find_first_of( ' ' );
			string cmd = msg.substr( 1, pos-1 );
			StringUtil::ToLower( cmd );

			string param = "";

			if( pos != string::npos && pos+1 < msg.length() )
				param = msg.substr( pos+1 );

			Command( L, code, cmd.c_str(), user.c_str(), param.c_str() );
		}
		else if( code == ROOM_MESSAGE )
		{
			/* if someone said our name, answer back */
			if( msg.length() == 7 || (msg.length() == 8 && ispunct(msg[7])) )
			{
				string name = msg.substr(0, 7);
				StringUtil::ToLower( name );

				if( name == "meepbot" )
					Say( "Yes, my Love?" );
				else if( name == "m33pb07" )
					Say( "y35, my l0v3?" );
			}
		}
		break;

	case WALL_MESSAGE:
 		printf( "[%s] %s\n", user.c_str(), packet.sMessage.c_str() );
		break;

	case CLIENT_IDLE:
		if( user == "MeepBot" )
			Write( ChatPacket(CLIENT_BACK) );
		break;

	/* handled, but ignored */
	case MOD_CHAT:
	case CLIENT_CONFIG:
	case CLIENT_BACK:
	case CLIENT_AWAY: 
	case 19:	/* bell? */
	case START_TYPING:
	case STOP_TYPING:
	case RESET_TYPING:
	case ROOM_ACTION:
		break;

	default:
		printf( "doesn't handle code %d\n", packet.iCode );
	}
}

int MeepBot::Read()
{
	/* always block on socket I/O; we can afford to wait. */
	int ret = m_Socket.Read( m_sReadBuffer, BUFFER_SIZE, false );

	if( ret < 0 )
	{
		printf( "read error: %s\n", strerror(errno) );
		m_bLoggedIn = false;
	}

	return ret;
}

int MeepBot::Write( const ChatPacket &packet )
{
	int ret = m_Socket.Write( packet.ToString(), false );

	if( ret <= 0 )
	{
		printf( "write error: %s\n", strerror(errno) );
		m_bLoggedIn = false;
	}

	return ret;
}
	
/*
 * Lua binding code
 */

static int Say( lua_State *L )
{
	if( lua_isstring(L, -1) )
		BOT->Say( lua_tostring(L, -1) );
	else
		printf( "couldn't say: not a string\n" );

	return 0;
}

static int Emote( lua_State *L )
{
	if( lua_isstring(L, -1) )
		BOT->Emote( lua_tostring(L, -1) );
	else
		printf( "couldn't emote: not a string\n" );

	return 0;
}

static int PM( lua_State *L )
{
	const char *name = lua_tostring( L, -2 );
	const char *msg = lua_tostring( L, -1 );

	if( name == NULL || msg == NULL )
	{
		printf( "no PM for you\n" );
		return 0;
	}

	BOT->PM( name, msg );
	return 0;
}

static int Rand( lua_State *L )
{
	unsigned ret = BOT->m_pISAAC->GetValue();

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

	lua_pushstring( L, BOT->m_UserList.Resolve(caller, pattern).c_str() );
	return 1;
}

static const luaL_reg bot_funcs[] =
{
	{ "Say",	Say },
	{ "Emote",	Emote },
	{ "PM",		PM },
	{ "Rand",	Rand },
	{ "Resolve",	Resolve },
	{ NULL, NULL },
};

/* temp timing stuff */
#include <sys/time.h>

uint64_t GetTimeElapsed( struct timeval &start, struct timeval &end )
{
	uint64_t ustart = start.tv_sec * 1000000 + start.tv_usec;
	uint64_t uend = end.tv_sec * 1000000 + end.tv_usec;
	return uend - ustart;
}

bool MeepBot::Command( lua_State *L, int type, const char *cmd, const char *caller, const char *params )
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
		bool ret = ReloadLua( err );
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

	if( lua_pcall( L, 3, 0, 0 ) != 0 )
	{
		printf( "Failed to run command \"%s\": %s\n", cmd, lua_tostring(L, -1) );
		return false;
	}

	LuaUtil::CleanStack( L );
	return true;
}

struct PacketType { MessageCode type; const char *name; };

const struct PacketType LuaPacketTypes[] =
{
	{ ROOM_MESSAGE, "TYPE_CHAT" },
	{ USER_PM,	"TYPE_PM" },
	{ MOD_CHAT,	"TYPE_MOD_CHAT" },

	/* arbitrary value, iteration sentinel */
	{ USER_JOIN,	NULL }
};

static void RegisterTypeGlobals( lua_State *L )
{
	/* register some important enumerations into the global namespace.
	 * the first argument to every command is 'type', the method by 
	 * which the command was called. */
	for( unsigned i = 0; true; ++i )
	{
		const struct PacketType &t = LuaPacketTypes[i];
		if( t.name == NULL )
			break;

		lua_pushnumber( L, t.type );
		lua_setfield( L, LUA_GLOBALSINDEX, t.name );

		printf( "registered %d -> %s\n", t.type, t.name );
	}
}

void MeepBot::Register( lua_State *L ) const
{
	/* push MeepBot to the stack */
	lua_getglobal(L, "MeepBot");

	/* if it already exists, remove and rebuild */
	if( !lua_isnil(L, -1) )
	{
		printf( "MeepBot is not nil, destroying\n" );

		/* set the Commands table nil (to wipe the reference) */
		lua_pushstring( L, "Commands" );
		lua_pushnil( L );
		lua_settable( L, -3 );

		/* pop the MeepBot table reference */
		lua_pop( L, -1 );

		lua_pushstring( L, "MeepBot" );
		lua_pushnil( L );
		lua_settable( L, LUA_GLOBALSINDEX );
	}

	/* pop the global MeepBot table */
	lua_pop( L, -1 );

	/* push our functions to the MeepBot table */
	luaL_register( L, "MeepBot", bot_funcs );

	/* create the Commands sub-table */
	lua_pushstring( L, "Commands" );
	lua_newtable( L );
	lua_settable( L, -3 );

	/* remove the MeepBot table from the stack */
	lua_pop( L, -1 );

	/* register a few important enums in the global namespace */
	RegisterTypeGlobals( L );
}
