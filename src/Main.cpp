#include <cstdio>
#include <cassert>
#include <csignal>
#include <lua5.1/lua.hpp>
#include "MeepBot.h"
#include "MeepBot_LuaBindings.h"
#include "packet/MessageCodes.h"	// for ROOM_MESSAGE in reload
#include "util/StringUtil.h"

using namespace std;

/* sets itself to BOT on initialization */
static MeepBot g_Bot;

static unsigned exit_lock = 0;

/* If we receive SIGHUP, reload scripts (and pretend it's me) */
void reload_scripts( int signum )
{
	MeepBot_LuaBindings::DoReload( ROOM_MESSAGE, "Fire_Adept" );
}

void clean_exit( int signum )
{
	if( exit_lock > 0 )
	{
		printf( "Alright, alright. Exiting now.\n" );
		exit( 1 );
	}

	printf( "Received signal %d: %s\n", signum, strsignal(signum) );

	/* intentional or accidental? */
	if( signum == SIGINT )
	{
		BOT->Emote( "is pulled into the inky shadows..." );
	}
	else
	{
		string str = StringUtil::Format( "Aaaahh! %s! Every man for himself!", strsignal(signum) );
		BOT->Say( str.c_str() );
	}

	BOT->Logout();
	++exit_lock;
}

char* GetFileContents( const char *path )
{
	FILE *pFile = fopen( path, "r" );

	if( pFile == NULL )
		return NULL;

	fseek( pFile, 0, SEEK_END );
	unsigned len = ftell( pFile );
	rewind( pFile );

	char *ret = new char[len+1];
	fread( ret, len, sizeof(char), pFile);
	ret[len] = '\0';
	return ret;
}

/* we exit cleanly on receiving these signals - 0 is an ending sentinel. */
const int signals[] = { SIGABRT, SIGFPE, SIGILL, SIGINT, SIGPIPE, SIGSEGV, SIGTERM, 0 };

int main()
{
	assert( BOT );

	/* set up our signal handling for cleaner exit */
	for( unsigned i = 0; signals[i] != 0; ++i )
		signal( signals[i], clean_exit );

	/* if we receive SIGHUP, reload scripts */
	signal( SIGHUP, reload_scripts );

	string err;
	if( !BOT->OpenLua(err) )
	{
		printf( "Failed to open Lua: %s\n", err.c_str() );
		return 1;
	}

	/* nothing personal, but I like not compromising the bot's account.
	 * feel free to replace these with string constants in your build. */
	const char* USERNAME = GetFileContents("/home/mark/.MeepBot/usr");
	const char *PASSWORD = GetFileContents("/home/mark/.MeepBot/pwd");

	unsigned wait_length = 1;

	while( exit_lock < 1 )
	{
		if( wait_length != 1 )
		{
			printf( "Waiting %d seconds to reconnect.\n", wait_length );
			sleep( wait_length );
		}

		wait_length *= 2;

		// bound up at half an hour
		if( wait_length > 1800 )
			wait_length = 1800;

		if( !BOT->Connect("runevillage.com", 7005) )
			continue;

		printf( "Attempting to log in...\n" );
		if( !BOT->Login(USERNAME, PASSWORD) )
			continue;
		
		BOT->MainLoop();
		BOT->Logout();
		BOT->Disconnect();

		wait_length = 1;	/* reset */
	}

	delete[] USERNAME;
	delete[] PASSWORD;

	BOT->Logout();

	return 0;
}
