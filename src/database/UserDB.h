#ifndef USER_DB_H
#define USER_DB_H

#include <cstdarg>
#include "SQLiteDB.h"

const char* const USER_TABLE = "users";

/* User DB layout (version 1):
 * CREATE TABLE users(
 *	userName TEXT UNIQUE PRIMARY KEY COLLATE NOCASE,
 *	accessLevel TINYINT,
 *	description TEXT)
 */

enum AccessLevel
{
	LEVEL_BANNED,	/* not allowed to use us at all */
	LEVEL_USER,	/* casual usage, some commands */
	LEVEL_OP,	/* add quotes, send PMs, etc. */
	LEVEL_MOD,	/* mod commands, manage ops and data */
	LEVEL_ADMIN	/* manage the bot, manage all other users */
};

class UserDB: public SQLiteDB
{
public:
	AccessLevel GetAccessLevel( const char *username );
	bool SetAccessLevel( const char *username, AccessLevel level );

	const char* GetDescription( const char *username );
	bool SetDescription( const char *name, const char *desc );

	bool Protect( bool bProtect );
};

#endif // USER_DB_H
