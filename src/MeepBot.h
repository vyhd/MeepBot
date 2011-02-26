#ifndef MEEPBOT_H
#define MEEPBOT_H

#include "model/UserList.h"
#include "network/Socket.h"
#include "packet/ChatPacket.h" 
#include <string>

/* a bunch of forward declarations so we don't need their headers here */
struct lua_State;
struct sqlite3;
class ISAAC;
class MeepBot;
class QuotesDB;
class UserDB;

/* This class acts as a singleton, since, well, it's only meant to be
 * instantiated once. Use BOT to access global methods from outside. */
extern MeepBot* BOT;

class MeepBot
{
public:
	MeepBot();
	~MeepBot();

	/* loads, unloads, reloads Lua state and command bindings */
	bool OpenLua( std::string &err );
	void CloseLua();
	bool ReloadLua( std::string &err );

	bool Connect( const char *server, unsigned port );
	void Disconnect();
	bool Login( const char *name, const char *pwd );
	void Logout();

	void MainLoop();
	void HandlePacket( const char *str );

	/* these do the obvious */
	void Say( const char *msg );
	void Emote( const char *msg );
	void PM( const char *user, const char *msg );

	/* these normally would be private, but Lua needs access to them
	 * and I don't feel like jumping through hoops to make it pretty */

	int Write( const ChatPacket &packet );

	UserList m_UserList;
	ISAAC *m_pISAAC;

	sqlite3* m_pDatabase;
	QuotesDB *m_pQuotesDB;
	UserDB *m_pUserDB;

	lua_State *L;

private:
	void Register( lua_State *L ) const;

	/* server I/O mechanisms */
	bool m_bLoggedIn;

	int Read();
	static const unsigned BUFFER_SIZE = 4096;
	char m_sReadBuffer[BUFFER_SIZE];

	Socket m_Socket;
};

#endif // MEEPBOT_H
