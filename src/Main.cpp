#include <cstdio>
#include <cassert>
#include <csignal>
#include <lua5.1/lua.hpp>
#include "MeepBot.h"
#include "util/StringUtil.h"

using namespace std;

/* sets itself to BOT on initialization */
static MeepBot g_Bot;

static unsigned exit_lock = 0;

void clean_exit( int signum )
{
	if( exit_lock > 0 )
		return;

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
	printf( "len: %d, ret: %s.\n", len, ret );
	return ret;
}

/* we exit cleanly on receiving these signals - 0 to signal end. */
const int signals[] = { SIGABRT, SIGFPE, SIGILL, SIGINT, SIGPIPE, SIGSEGV, SIGTERM, 0 };

int main()
{
	assert( BOT );

	/* set up our signal handling for cleaner exit */
	for( unsigned i = 0; signals[i] != 0; ++i )
		signal( signals[i], clean_exit );

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

	while( BOT->Connect("runevillage.com", 7005) )
	{
		if( !BOT->Login(USERNAME, PASSWORD) )
		{
			printf( "Couldn't login.\n" );
			return 1;
		}

		BOT->MainLoop();
		BOT->Logout();
		BOT->Disconnect();
		sleep( 5 );
	}

	delete[] USERNAME;
	delete[] PASSWORD;

	BOT->Logout();

	return 0;
}
