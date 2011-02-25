#ifndef USER_DB_H
#define USER_DB_H

const char* const USER_TABLE = "users";

struct sqlite3;

enum AccessLevel
{
	LEVEL_BANNED,	/* not allowed to use us at all */
	LEVEL_USER,	/* casual usage, some commands */
	LEVEL_OP,	/* add quotes, send PMs, etc. */
	LEVEL_MOD,	/* mod commands, manage ops and data */
	LEVEL_ADMIN	/* manage the bot, manage all other users */
};

/* we need this for UserEntry and GetDescription */
#include <string>

struct UserEntry
{
	UserEntry() { }
	UserEntry( std::string desc_, AccessLevel level_, bool locked_ ) :
		desc( desc_ ), level( level_ ), locked( locked_ ) { }

	std::string desc;
	AccessLevel level;
	bool locked;
};

class UserDB
{
public:
	UserDB( sqlite3 *db );

	bool AddUser( const char *name, const char *desc, AccessLevel level );
	bool HasUser( const char *name ) const;

	AccessLevel GetAccessLevel( const char *username ) const;
	bool SetAccessLevel( const char *username, AccessLevel level );

	std::string GetDescription( const char *username ) const;
	bool SetDescription( const char *name, const char *desc );

	/* bProtect = whether to protect the user's rank or not */
	bool Protect( bool bProtect );

	/* returns true if 'entry' could be filled with the user data */
	bool GetUserEntry( const char *name, UserEntry &entry ) const;

	/* inserts the data from the user entry into the database */
	bool SetUserEntry( const char *name, const UserEntry &entry );

private:
	sqlite3* m_pDB;
};

#endif // USER_DB_H
