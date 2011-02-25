#include "MeepBot.h"
#include "database/SQLite.h"
#include "database/QuotesDB.h"
#include "database/UserDB.h"
#include "packet/ChatPacket.h"
#include "packet/MessageCodes.h"
#include "packet/PacketUtil.h"
#include "util/ISAAC.h"
#include "util/LuaUtil.h"
#include "util/StringUtil.h"
#include <lua5.1/lua.hpp>

#include "MeepBot_LuaBindings.h"

#include <ctime>
#include <cerrno>
#include <cstring>

using namespace std;
struct sqlite3;

MeepBot *BOT = NULL;

/* directory from which Lua scripts are loaded */
const char* SCRIPTS_DIR = "scripts";

/* RGB values for the chat bot's messages */
const char RED = 92;
const char GREEN = 0;
const char BLUE = 168;

/* default level for users the bot hasn't seen before */
const AccessLevel DEFAULT_ACCESS_LEVEL = LEVEL_OP;

MeepBot::MeepBot()
{
	m_bLoggedIn = false;

	BOT = this;
	L = NULL;

	memset( m_sReadBuffer, 0, BUFFER_SIZE );

	/* Open a database connection and instantiate our databases. */
	m_pDatabase = SQLite::Open( "MeepBot.db" );
	m_pQuotesDB = new QuotesDB( m_pDatabase );
	m_pUserDB = new UserDB( m_pDatabase );

	/* lol I'm so clever :awesome: */
	unsigned int seed[8] = { 'M','E','E','P','B','O','T', time(NULL) };
	m_pISAAC = new ISAAC( seed );
}

MeepBot::~MeepBot()
{
	delete m_pISAAC;

	delete m_pQuotesDB;
	SQLite::Close( m_pDatabase );
}

void MeepBot::Say( const char *str )
{
	if( !m_bLoggedIn || strlen(str) == 0 )
		return;

	printf( "[MeepBot] %s\n", str );
	Write( ChatPacket(ROOM_MESSAGE, BLANK, str, RED, GREEN, BLUE) );
}

void MeepBot::Emote( const char *str )
{
	if( !m_bLoggedIn || strlen(str) == 0 )
		return;

	printf( "**MeepBot %s\n", str );
	Write( ChatPacket(ROOM_ACTION, BLANK, str) );
}

void MeepBot::PM( const char *name, const char *msg )
{
	if( !m_bLoggedIn || strlen(msg) == 0 )
		return;

	printf( "[MeepBot] -> [%s] %s\n", name, msg );
	/* TODO: verify that a user is actually on the server first */
	Write( ChatPacket(USER_PM, name, msg) );
}

bool MeepBot::OpenLua( std::string &err )
{
	/* initialize the internal Lua state and run all our scripts */
	L = LuaUtil::Open();

	MeepBot_LuaBindings::Register( L );

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

void MeepBot::Disconnect()
{
	m_Socket.Close();
}

bool MeepBot::Login( const char *name, const char *pwd )
{
	ChatPacket req = ChatPacket(USER_JOIN, name, pwd);

	/* send our login message, wait for the response */
	if( Write(req) > 0 && Read() )
	{
		int response = ChatPacket(m_sReadBuffer).iCode;

		if( response == ACCESS_GRANTED )
		{
			m_bLoggedIn = true;
			Say( "Greetings, Mortals." );

			/* request a list of currently chatting users */
			Write( ChatPacket(USER_LIST) );

			return true;
		}

		printf( "Denied: code %d\n", response );
	}

	return false;
}

void MeepBot::Logout()
{
	if( m_bLoggedIn )
		Write( ChatPacket(USER_PART, BLANK, BLANK) );

	m_bLoggedIn = false;
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
	case USER_JOIN:
		if( user == BLANK )
			break;

		m_UserList.Add( user );

		/* figure out this user's rank and add them to the bot */
		{
			unsigned pos = msg.find_first_of('|') + 1;

			const char *admin_ranks = "AC";
			const char *mod_ranks = "cbf!";
			AccessLevel level = DEFAULT_ACCESS_LEVEL;

			for( unsigned i = 0; i < strlen(admin_ranks); ++i )
				if( msg[pos] == admin_ranks[i] )
					level = LEVEL_ADMIN;

			for( unsigned i = 0; i < strlen(mod_ranks); ++i )
				if( msg[pos] == mod_ranks[i] )
					level = LEVEL_MOD;

			UserEntry entry( "", level, false );
			m_pUserDB->SetUserEntry( user.c_str(), entry );
		}
		break;
	case USER_PART:
		m_UserList.Remove( user );
		break;
	case USER_MUTE:
		if( user == "MeepBot" )
		{
			Write( ChatPacket(USER_UNMUTE, BLANK, "MeepBot") );
			Say( "Muting MeepBot is mean :(" );
		}
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

			MeepBot_LuaBindings::Command( L, code, cmd.c_str(), user.c_str(), param.c_str() );
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

	/* ignored */
	case USER_UNMUTE:
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
