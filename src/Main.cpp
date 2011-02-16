#include <cstdio>
#include <cassert>
#include <signal.h>
#include <lua5.1/lua.hpp>
#include "MeepBot.h"

using namespace std;

/* sets itself to BOT on initialization */
static MeepBot g_Bot;

static unsigned exit_lock = 0;

/* only handle this one */
void clean_exit( int signum )
{
	if( exit_lock > 0 )
		return;

	BOT->Emote( "is pulled into the inky shadows..." );
	BOT->Logout();
	++exit_lock;
}

char* GetFileContents( const char *path )
{
	FILE *pFile = fopen( path, "rb" );

	if( pFile == NULL )
		return NULL;

	fseek( pFile, 0, SEEK_END );
	unsigned len = ftell( pFile );

	char *ret = new char[len];
	fread( ret, len, sizeof(char), pFile);
	return ret;
}

int main()
{
	assert( BOT );

	/* set up our signal handling for cleaner exit */
	signal( SIGPIPE, clean_exit );
	signal( SIGSEGV, clean_exit );
	signal( SIGINT, clean_exit );
	signal( SIGTERM, clean_exit );
	signal( SIGABRT, clean_exit );

	string err;
	if( !BOT->OpenLua(err) )
	{
		printf( "Failed to open Lua: %s\n", err.c_str() );
		return 1;
	}

	BOT->Connect( "runevillage.com", 7005 );

	/* nothing personal, but I like not compromising the bot's account.
	 * feel free to replace these with string constants in your build. */

	const char* USERNAME = GetFileContents("/home/mark/.MeepBot/usr");
	const char *PASSWORD = GetFileContents("/home/mark/.MeepBot/pwd");

	if( !BOT->Login(USERNAME, PASSWORD) )
		return 1;

	BOT->MainLoop();
	BOT->Logout();

	return 0;
}
