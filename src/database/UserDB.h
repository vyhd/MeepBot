#ifndef USER_DB_H
#define USER_DB_H

const char* const USER_TABLE = "users";

struct sqlite3;
struct UserEntry;

enum AccessLevel
{
	LEVEL_BANNED,	/* not allowed to use us at all */
	LEVEL_USER,	/* casual usage, some commands */
	LEVEL_OP,	/* add quotes, send PMs, etc. */
	LEVEL_MOD,	/* mod commands, manage ops and data */
	LEVEL_ADMIN	/* manage the bot, manage all other users */
};

class UserDB
{
public:
	UserDB( sqlite3 *db );

	AccessLevel GetAccessLevel( const char *username );
	bool SetAccessLevel( const char *username, AccessLevel level );

	const char* GetDescription( const char *username );
	bool SetDescription( const char *name, const char *desc );

	bool Protect( bool bProtect );

private:
	/* Returns true and fills out entry if the user's entry exists */
	bool GetUserEntry( const char *username, UserEntry *entry );

	sqlite3* m_pDB;
};

#endif // USER_DB_H
